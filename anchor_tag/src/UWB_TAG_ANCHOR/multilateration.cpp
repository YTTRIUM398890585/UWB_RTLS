#ifdef IS_TAG

#include "UWB_TAG_ANCHOR/multilateration.h"

// TODO: error handling for the case of not enough anchors, or colinear anchors

/**
 * @brief compute the position of the tag using multilateration
 *
 * @param mul_data linked list of anchors
 * @param debugPrints to print debug information
 * @return Eigen::Vector3f
 */
Vector3f multilateration(AnchorLinkedList& mul_data, bool debugPrints)
{
	// Multilateration /////////////////////////////////////
	// A = [2(x0-xi) 2(y0-yi) 2(z0-zi)] for anchor row i = 0, 1, 2, ...
	// d = [di^2 -d0^2 - xi^2 + x0^2 - yi^2 + y0^2 - zi^2 + z0^2 ] for anchor row i = 0, 1, 2, ...
	// r = [x y z]

	Matrix<float, Dynamic, 3> A;
	Matrix<float, Dynamic, 1> d;
	Vector3f r;
	r << 0, 0, 0;

	// total number of anchors
	size_t num_anchors = mul_data.getSize();

	// only continue if there are atleast 3 anchors
	if (num_anchors >= 3) {
		// get the pointer to the dummy node
		AnchorLinkedList::AnchorNode* temp = mul_data.getHead();

		// set temp to be the first anchor after the dummy node, and make this the reference anchor
		// reference anchor is used to minus the distance from the other anchors to linearise the system
		temp = temp->next;
		AnchorLinkedList::AnchorNode* reference_anchor = temp;

        // calculate the average distance of the reference anchor
        float reference_distance = filterAverageDistance(reference_anchor->distance);

		// since the reference anchor is used to minus the distance from the other anchors:
		// dimension of A is num_anchors - 1 by 3
		// dimension of d is num_anchors - 1 by 1
		A.resize(num_anchors - 1, 3);
		d.resize(num_anchors - 1, 1);

		// fill in the A with the anchor locations information
		// fill in d with the distance between anchors
		for (auto i = 0; i < num_anchors - 1; i++) {
			// skip dummy node and reference_anchor
			A(i, 0) = 2 * (reference_anchor->anchor_coords[0] - temp->next->anchor_coords[0]);
			A(i, 1) = 2 * (reference_anchor->anchor_coords[1] - temp->next->anchor_coords[1]);
			A(i, 2) = 2 * (reference_anchor->anchor_coords[2] - temp->next->anchor_coords[2]);

            // calculate the average distance of the anchor
            float distance = filterAverageDistance(temp->next->distance);

			// clang-format off
            d(i) = pow(distance, 2) - pow(reference_distance, 2)
                - pow(temp->next->anchor_coords[0], 2) + pow(reference_anchor->anchor_coords[0], 2)
                - pow(temp->next->anchor_coords[1], 2) + pow(reference_anchor->anchor_coords[1], 2)
                - pow(temp->next->anchor_coords[2], 2) + pow(reference_anchor->anchor_coords[2], 2);
			// clang-format on

			temp = temp->next;
		}

		if (debugPrints) {
			printMatrix(A, "A");
			printVector(d, "d");
		}

		// if num_anchors < 3, give up

		// if system is under determined <=> num_anchors = 3
		// r = AT (AAT)^-1 d

		// if system is over determined or invertible <=> num_anchors >= 4
		// r = (ATA)^-1 AT d

		if (num_anchors < 3) {
			if (debugPrints) {
				Serial.println("Not enough anchors to calculate position");
			}
		}
		else if (num_anchors == 3) {
			// this is only for 2D case
			// A is a 2 by 3 matrix
			// existance of right inverse is guaranteed iff anchors are not colinear, rank(A) = 2
			r = A.transpose() * (A * A.transpose()).inverse() * d;

			if (debugPrints) {
				Serial.println("under determined");
				printVector(r, "r");
			}
		}
		else {
			// A is a i by 3 matrix, where i >= 3
			// for the case of even system, A is a square matrix, invertibility is guaranteed iff anchors are not
			// colinear, rank(A) = 3
			//
			// for the case of over determined system, A is a tall matrix, left inverse is
			// guaranteed as long as 3 anchors are not colinear, rank(A) = 3
			r = (A.transpose() * A).inverse() * A.transpose() * d;

			if (debugPrints) {
				Serial.println("even or over determined");
				printVector(r, "r");
			}
		}
	}

	return r;
}

/**
 * @brief perform filtering of outliers and find average of the distances
 * 
 * @param p_distances pointer to the array of distances
 * @return float 
 */
float filterAverageDistance(float *p_distances) {
    // Sort the array using Bubble Sort
    float temp;
    for (size_t i = 0; i < AnchorLinkedList::MAX_BUFFER_SIZE - 1; i++) {
        for (size_t j = 0; j < AnchorLinkedList::MAX_BUFFER_SIZE - i - 1; j++) {
            if (p_distances[j] > p_distances[j + 1]) {
                temp = p_distances[j];
                p_distances[j] = p_distances[j + 1];
                p_distances[j + 1] = temp;
            }
        }
    }

    // Calculate the median
    float median;
    if (AnchorLinkedList::MAX_BUFFER_SIZE % 2 == 0) {
        median = (p_distances[AnchorLinkedList::MAX_BUFFER_SIZE / 2 - 1] + p_distances[AnchorLinkedList::MAX_BUFFER_SIZE / 2]) / 2.0f;
    } else {
        median = p_distances[AnchorLinkedList::MAX_BUFFER_SIZE / 2];
    }

    // Calculate the average of values that is within 10% of median
    float sum = 0;
    uint8_t count = 0; 

    for (size_t i = 0; i < AnchorLinkedList::MAX_BUFFER_SIZE; i++) {
        if (p_distances[i] > 0.9 * median && p_distances[i] < 1.1 * median) {
            sum += p_distances[i];
            count++;
        }
    }
    return sum / float(count);
}
#endif