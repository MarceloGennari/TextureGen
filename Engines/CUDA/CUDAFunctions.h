#include <../../Utils/objects.h>
#include <cuda_runtime_api.h>
#include <cuda.h>

__global__ void findTexCoords(std::vector<std::vector<float> > *depthPixels, std::vector<std::vector<std::vector<Vertex *> > > *verticesInPixel, int *fr, int *frNr);
void findTexCoord(std::vector<std::vector<float> > *depthPixels, std::vector<std::vector<std::vector<Vertex *> > > *PixelsInImage, int *fr, int *frNr);
