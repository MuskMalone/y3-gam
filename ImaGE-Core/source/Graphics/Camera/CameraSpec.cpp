#include <pch.h>
#include "CameraSpec.h"
#define GRIBB_HARTMANN

namespace Graphics {
  void CameraSpec::ComputeFrustum() {
#ifdef GRIBB_HARTMANN
		// Gribb/Hartmann method
		// source: https://www8.cs.umu.se/kurser/5DV051/HT12/lab/plane_extraction.pdf
    frustum.leftP = { viewProjMatrix[0][3] + viewProjMatrix[0][0],
											viewProjMatrix[1][3] + viewProjMatrix[1][0],
											viewProjMatrix[2][3] + viewProjMatrix[2][0],
											viewProjMatrix[3][3] + viewProjMatrix[3][0]
    };
    frustum.rightP = { viewProjMatrix[0][3] - viewProjMatrix[0][0],
											 viewProjMatrix[1][3] - viewProjMatrix[1][0],
											 viewProjMatrix[2][3] - viewProjMatrix[2][0],
											 viewProjMatrix[3][3] - viewProjMatrix[3][0]
    };

    frustum.topP = { viewProjMatrix[0][3] - viewProjMatrix[0][1],
										 viewProjMatrix[1][3] - viewProjMatrix[1][1],
										 viewProjMatrix[2][3] - viewProjMatrix[2][1],
										 viewProjMatrix[3][3] - viewProjMatrix[3][1]
    };
    frustum.btmP = { viewProjMatrix[0][3] + viewProjMatrix[0][1],
										 viewProjMatrix[1][3] + viewProjMatrix[1][1],
										 viewProjMatrix[2][3] + viewProjMatrix[2][1],
										 viewProjMatrix[3][3] + viewProjMatrix[3][1]
    };

    frustum.nearP = { viewProjMatrix[0][3] + viewProjMatrix[0][2],
											viewProjMatrix[1][3] + viewProjMatrix[1][2],
											viewProjMatrix[2][3] + viewProjMatrix[2][2],
											viewProjMatrix[3][3] + viewProjMatrix[3][2]
    };
    frustum.farP = { viewProjMatrix[0][3] - viewProjMatrix[0][2],
										 viewProjMatrix[1][3] - viewProjMatrix[1][2],
										 viewProjMatrix[2][3] - viewProjMatrix[2][2],
										 viewProjMatrix[3][3] - viewProjMatrix[3][2]
    };

		/*for (int i{}; i < 6; ++i) {
			frustum[i].Normalize();
		}*/
#else	// requires projection mtx to be passed in
		// source: https://github.com/EQMG/Acid/blob/master/Sources/Physics/Frustum.cpp
		std::array<float, 16> clip;

		clip[0] = view[0][0] * projection[0][0] + view[0][1] * projection[1][0] + view[0][2] * projection[2][0] + view[0][3] * projection[3][0];
		clip[1] = view[0][0] * projection[0][1] + view[0][1] * projection[1][1] + view[0][2] * projection[2][1] + view[0][3] * projection[3][1];
		clip[2] = view[0][0] * projection[0][2] + view[0][1] * projection[1][2] + view[0][2] * projection[2][2] + view[0][3] * projection[3][2];
		clip[3] = view[0][0] * projection[0][3] + view[0][1] * projection[1][3] + view[0][2] * projection[2][3] + view[0][3] * projection[3][3];

		clip[4] = view[1][0] * projection[0][0] + view[1][1] * projection[1][0] + view[1][2] * projection[2][0] + view[1][3] * projection[3][0];
		clip[5] = view[1][0] * projection[0][1] + view[1][1] * projection[1][1] + view[1][2] * projection[2][1] + view[1][3] * projection[3][1];
		clip[6] = view[1][0] * projection[0][2] + view[1][1] * projection[1][2] + view[1][2] * projection[2][2] + view[1][3] * projection[3][2];
		clip[7] = view[1][0] * projection[0][3] + view[1][1] * projection[1][3] + view[1][2] * projection[2][3] + view[1][3] * projection[3][3];

		clip[8] = view[2][0] * projection[0][0] + view[2][1] * projection[1][0] + view[2][2] * projection[2][0] + view[2][3] * projection[3][0];
		clip[9] = view[2][0] * projection[0][1] + view[2][1] * projection[1][1] + view[2][2] * projection[2][1] + view[2][3] * projection[3][1];
		clip[10] = view[2][0] * projection[0][2] + view[2][1] * projection[1][2] + view[2][2] * projection[2][2] + view[2][3] * projection[3][2];
		clip[11] = view[2][0] * projection[0][3] + view[2][1] * projection[1][3] + view[2][2] * projection[2][3] + view[2][3] * projection[3][3];

		clip[12] = view[3][0] * projection[0][0] + view[3][1] * projection[1][0] + view[3][2] * projection[2][0] + view[3][3] * projection[3][0];
		clip[13] = view[3][0] * projection[0][1] + view[3][1] * projection[1][1] + view[3][2] * projection[2][1] + view[3][3] * projection[3][1];
		clip[14] = view[3][0] * projection[0][2] + view[3][1] * projection[1][2] + view[3][2] * projection[2][2] + view[3][3] * projection[3][2];
		clip[15] = view[3][0] * projection[0][3] + view[3][1] * projection[1][3] + view[3][2] * projection[2][3] + view[3][3] * projection[3][3];

		// This will extract the LEFT side of the frustum.
		frustum.leftP = { clip[3] - clip[0], clip[7] - clip[4], clip[11] - clip[8], clip[15] - clip[12] };

		frustum.rightP = { clip[3] + clip[0] ,clip[7] + clip[4], clip[11] + clip[8] ,clip[15] + clip[12] };

		frustum.btmP = { clip[3] + clip[1], clip[7] + clip[5], clip[11] + clip[9], clip[15] + clip[13] };

		frustum.topP = { clip[3] - clip[1], clip[7] - clip[5], clip[11] - clip[9], clip[15] - clip[13] };

		frustum.nearP = { clip[3] + clip[2], clip[7] + clip[6], clip[11] + clip[10], clip[15] + clip[14] };

		frustum.farP = { clip[3] - clip[2], clip[7] - clip[6], clip[11] - clip[10], clip[15] - clip[14] };

		/*for (int i{}; i < 6; ++i) {
			frustum[i].Normalize();
		}*/
#endif
  }
}
