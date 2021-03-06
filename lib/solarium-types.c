#include <assert.h>
#include <malloc.h>
#include <math.h>
#include "solarium-types.h"
#include "solarium-draw.h"
#include "i2c/device.h"

#define DEGREES_PER_RING 5.625

uint8_t BAD_ADDRESSES[] = { 112 };

// Each PCA9635
device_t device[NUM_DEVICES];

// All the beams in the Solarium
beam_t beam[NUM_BEAMS];

// All the rays in the Solarium.
ray_t  ray[NUM_RAYS];

// Each ring
ring_t ring[NUM_RINGS];

int valid_device_addr (uint8_t addr);
void set_ring_size (ring_t *ring, uint8_t num_beams, int *beam_index, int lat);
void assign_ray (ring_t *ring, uint8_t ray_num);
void assign_partial_ray (ring_t *ring, uint8_t ray_num, uint8_t first, uint8_t last);

void setup (void) {
	int x, y, z;
	uint8_t device_addr = FIRST_DEVICE_ADDRESS;
	// Zero out the rays and assign devices
	for (x = 0; x < NUM_RAYS; ++x) {
		ray[x].id = x + 1;
		ray[x].dirty = 1;

		// Assign the three devices and set pointers
		for (y = 0; y < 3; ++y) {
			assert(((3*x) + y) < NUM_DEVICES);

			// Save a pointer to the next available device
			ray[x].devices[y] = &device[(3*x)+y];
			
			// Save a pointer to this device's ray's dirty bit
			device[(3*x) + y].dirty = &(ray[x].dirty);

			// Set the address for the device
			if (device_addr == 112) {
				device[(3*x) + y].addr = 126;
			}
			else {
				device[(3*x) + y].addr = device_addr;
			}

			// Initialize each of the 16 LEDs the device controls
			for (z = 0; z < 16; ++z) {
				// Zero out the value
				device[(3*x) + y].value[z] = 0;
			}

                        device_addr++;
		}
		// Beams are assigned to rays in assign_partial_ray
	}

	/** Index into the beam array **/
	int beam_index = 0;

	/*************************************************************/
	/* Size the rings and link rays/beams to rings, ring by ring */

	/** Ring 16 - 60 beams **/
	set_ring_size(&ring[16], 60, &beam_index, 0);
	assign_ray(&ring[16], 1);
	assign_ray(&ring[16], 2);
	assign_ray(&ring[16], 3);
	assign_partial_ray(&ring[16], 4, 1, 12);

	/** Ring 15 - 58 beams **/
	set_ring_size(&ring[15], 58, &beam_index, 6); //5.625
	assign_ray(&ring[15], 5);
	assign_ray(&ring[15], 6);
	assign_ray(&ring[15], 7);
	assign_partial_ray(&ring[15], 8, 1, 10);

	/** Ring 14 - 56 beams **/
	set_ring_size(&ring[14], 56, &beam_index, 11); // 11.25
	assign_ray(&ring[14], 9);
	assign_ray(&ring[14], 10);
	assign_ray(&ring[14], 11);
	assign_partial_ray(&ring[14], 15, 11, 16);
	assign_partial_ray(&ring[14], 8, 11, 12);

	/** Ring 13 - 53 beams **/
	set_ring_size(&ring[13], 53, &beam_index, 17); // 16.875
	assign_ray(&ring[13], 12);
	assign_ray(&ring[13], 13);
	assign_ray(&ring[13], 14);
	assign_partial_ray(&ring[13], 15, 1, 5);

	/** Ring 12 - 50 beams **/
	set_ring_size(&ring[12], 50, &beam_index, 22); // 22.5
	assign_ray(&ring[12], 16);
	assign_ray(&ring[12], 17);
	assign_partial_ray(&ring[12], 18, 1, 13);
	assign_partial_ray(&ring[12], 15, 6, 10);

	/** Ring 11 - 46 beams **/
	set_ring_size(&ring[11], 46, &beam_index, 28); // 28.125
	assign_ray(&ring[11], 19);
	assign_ray(&ring[11], 20);
	assign_partial_ray(&ring[11], 21, 1, 11);
	assign_partial_ray(&ring[11], 18, 14, 16);

	/** Ring 10 - 43 beams **/
	set_ring_size(&ring[10], 43, &beam_index, 34); // 33.75
	assign_ray(&ring[10], 22);
	assign_ray(&ring[10], 23);
	assign_partial_ray(&ring[10], 26, 11, 16);
	assign_partial_ray(&ring[10], 21, 12, 16);

	/** Ring 9 - 39 beams **/
	set_ring_size(&ring[9], 39, &beam_index, 39); // 39.375
	assign_ray(&ring[9], 24);
	assign_ray(&ring[9], 25);
	assign_partial_ray(&ring[9], 26, 1, 7);

	/** Ring 8 - 35 beams **/
	set_ring_size(&ring[8], 35, &beam_index, 45); // 45
	assign_ray(&ring[8], 27);
	assign_ray(&ring[8], 28);
	assign_partial_ray(&ring[8], 26, 8, 10);

	/** Ring 7 - 31 beams **/
	set_ring_size(&ring[7], 31, &beam_index, 51); // 50.625
	assign_ray(&ring[7], 29);
	assign_partial_ray(&ring[7], 30, 1, 15);

	/** Ring 6 - 27 beams **/
	set_ring_size(&ring[6], 27, &beam_index, 56); // 56.25
	assign_ray(&ring[6], 31);
	assign_partial_ray(&ring[6], 32, 1, 10);
	assign_partial_ray(&ring[6], 30, 16, 16);

	/** Ring 5 - 22 beams **/
	set_ring_size(&ring[5], 22, &beam_index, 62); // 61.875
	assign_ray(&ring[5], 33);
	assign_partial_ray(&ring[5], 32, 11, 16);

	/** Ring 4 - 18 beams **/
	set_ring_size(&ring[4], 18, &beam_index, 68); // 67.5
	assign_ray(&ring[4], 34);
	assign_partial_ray(&ring[4], 35, 15, 16);

	/** Ring 3 - 14 beams **/
	set_ring_size(&ring[3], 14, &beam_index, 73); // 73.125
	assign_partial_ray(&ring[3], 35, 1, 14);

	/** Ring 2 - 9 beams **/
	set_ring_size(&ring[2], 9, &beam_index, 79); // 78.75
	assign_partial_ray(&ring[2], 36, 6, 14);
	
	/** Ring 1 - 4 beams **/
	set_ring_size(&ring[1], 4, &beam_index, 84); // 84.375
	assign_partial_ray(&ring[1], 36, 2, 5);

	/** Ring 0 - 1 beam **/
	set_ring_size(&ring[0], 1, &beam_index, 90);
	assign_partial_ray(&ring[0], 36, 1, 1);

	// init the devices
	init_devices();

	// Draw the 0's out to the devices, just to make sure
	draw();
}

void set_ring_size (ring_t *ring, uint8_t num_beams, int *beam_index, int lat) {
	int x;

	ring->num_beams = num_beams;
	ring->beams     = (beam_t **) malloc(num_beams * sizeof(beam_t*));

	for (x = 0; x < num_beams; x++) {
		assert (*beam_index < NUM_BEAMS && *beam_index >= 0);

		ring->beams[x] = &(beam[*beam_index]);
		/* Initialize the beam */
		beam[*beam_index].red = NULL;
		beam[*beam_index].green = NULL;
		beam[*beam_index].blue = NULL;
		beam[*beam_index].dirty = NULL;
		beam[*beam_index].position.elevation = lat * PI / 180;
		beam[*beam_index].position.ele_sin = sin(beam[*beam_index].position.elevation);
		beam[*beam_index].position.ele_cos = cos(beam[*beam_index].position.elevation);
		++(*beam_index);
	}
//	fprintf (stderr, "Setting beam numbers %d to %d to NULL (%d beams).\n", *beam_index - num_beams, *beam_index - 1, num_beams);
}

// Note, ray_num should start at 1 to match the hemisphere map.  It will be -1 in the function
// to map to the array properly.  Similarly first and last should start at 1
void assign_partial_ray (ring_t *ring, uint8_t ray_num, uint8_t first, uint8_t last) {
	unsigned int i = 0;
	int start = -1;
	int end;
	ray_t *r;

	// Find the next place to start adding beams
	while (i < ring->num_beams) {
		assert (ring->beams[i] != NULL);
		if (ring->beams[i]->dirty == NULL) {
			start = i;
			break;
		}
		++i;
	}

	// If we didn't find a start position, exit
	assert (start != -1);

	// Make sure we clip where needed
	end = last - first;

	assert (end < ring->num_beams);

	r = get_ray(ray_num);

	// Start assigning beams
	for (i = 0; i <= end; ++i) {
		/* What is being done here is not very intuitive.
		   At this point in initialization, beams are still not attached
		   to devices, but are attached to rings.  Since we are linking
		   up rays and rings, we can also figure out which devices map to
		   the beam that is shared between the ray and ring.
		*/
//		fprintf(stderr, "Initializing beam %d\n", start+i);
		ring->beams[start+i]->dirty = &(r->dirty);
		ring->beams[start+i]->red = &(r->devices[0]->value[i+first-1]);
		ring->beams[start+i]->green = &(r->devices[1]->value[i+first-1]);
		ring->beams[start+i]->blue = &(r->devices[2]->value[i+first-1]);

		// Assign the beam pointer for the ray to the same beam that the
		// ring is pointing to.
		r->beams[i+first-1] = ring->beams[start+i];

		ring->beams[start+i]->position.azimuth = 2 * PI * (start+i) / ring->num_beams;
		ring->beams[start+i]->position.az_sin = sin(ring->beams[start+i]->position.azimuth);
		ring->beams[start+i]->position.az_cos = cos(ring->beams[start+i]->position.azimuth);
	}
}

void assign_ray (ring_t *ring, uint8_t ray_num) {
	assign_partial_ray(ring, ray_num, 1, 16);
}

int valid_device_addr (uint8_t addr) {
	int i;
        assert (addr <= LAST_DEVICE_ADDRESS);
	for (i = 0; i < sizeof(BAD_ADDRESSES); ++i) {
		if (addr == BAD_ADDRESSES[i]) {
			return 0;
		}
	}
	return 1;
}

device_t *get_device(uint8_t index) {

	assert (index < NUM_DEVICES);

	return &device[index];
}

ray_t *get_ray(uint8_t index) {
        // The index argument is 1 indexed, but the array is 0 indexed
        --index;

	assert (index < NUM_RAYS);

	return &(ray[index]);
}

beam_t *get_beam(uint16_t index) {

	assert (index < NUM_BEAMS);

	return &(beam[index]);
}

ring_t *get_ring (uint8_t index)
{
	assert (index < NUM_RINGS);

	return &(ring[index]);
}
