#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if MLOpen_BACKEND_OPENCL
#include <CL/cl.h>
typedef cl_command_queue mlopenStream_t;

#elif MLOpen_BACKEND_HIP
#include <hip_runtime.h>
typedef hipStream_t mlopenStream_t;
#endif

typedef struct mlopenContext *mlopenHandle_t;

typedef enum {
	mlopenStatusSuccess = 0,
	mlopenStatusNotInitialized = 1,
	mlopenStatusInvalidValue = 2,
	mlopenStatusBadParm = 3,
	mlopenStatusAllocFailed = 4,
	mlopenStatusInternalError = 5,
	mlopenStatusNotImplemented = 6,
} mlopenStatus_t;

mlopenStatus_t mlopenCreate(mlopenHandle_t *handle, 
		mlopenStream_t				stream = NULL);

mlopenStatus_t mlopenDestroy(mlopenHandle_t handle);

// For OpenCL, stream is a command queue where as for HIP, it is a stream
mlopenStatus_t mlopenSetStream(mlopenHandle_t handle, 
		mlopenStream_t				streamId);

mlopenStatus_t mlopenGetStream(mlopenHandle_t handle, 
		mlopenStream_t				*streamId);

#if 0
// Create a Tensor Descriptor
// TODO: Do we need to pass the handle around for these APIs?
mlopenStatus_t mlopenCreateTensorDescriptor(mlopenHandle_t handle,
		mlopenTensorDescriptor_t	*tensorDesc);

// Only supporting NCHW for now and merging both expert and regular cuDNN APIs
mlopenStatus_t mlopenInit4dTensorDescriptor(mlopenHandle_t handle,
		mlopenTensorDescriptor_t	tensorDesc,
		mlopenDataType_t			datatype, // half/float/double
		int							n,
		int							c,
		int							h,
		int							w,
		int							nStride = 0,
		int							cStride = 0,
		int 						hStride = 0,
		int 						wStride = 0);

// Get the details of the tensor desciptor
mlopenStatus_t mlopenGet4dTensorDescriptor(mlopenHandle_t handle,
		mlopenTensorDescriptor_t	tensorDesc,
		mlopenDataType_t			*datatype,
		int							*n,
		int							*c, 
		int							*h,
		int							*w,
		int							*nStride,
		int							*cStride,
		int							*hStride,
		int							*wStride);

// Not sure if the following two APIs are required right now
mlopenStatus_t mlopenInitNdTensorDescriptor(mlopenHandle_t handle,
		mlopenTensorDescriptor_t	tensorDesc,
		mlopenDataType_t			datatype,
		int							nbDims,
		int							*dimA,
		int							*strideA);

// Get the details of the n-dimensional tensor desciptor
mlopenStatus_t mlopenGetNdTensorDescriptor(mlopenHandle_t handle,
		mlopenTensorDescriptor_t	tensorDesc,
		int							nbDimsRequested,
		mlopenDataType_t			*datatype,
		int							*nbDims,
		int							*dimA,
		int							*strideA);
		
mlopenStatus_t mlopenDestroyTensorDescriptor(mlopenTensorDescriptor_t tensorDesc);

/* This function copies the scaled data from one tensor to another
 * tensor with a different layout. Those descriptors need to have the
 * same dimensions but not necessarily the same strides. The input
 * and output tensors must not overlap in any way (i.e., tensors
 * cannot be transformed in place). This function can be used
 * to convert a tensor with an unsupported format to a supported one.
 */
mlopenStatus_t mlopenTransformTensor(mlopenHandle_t handle,
		const void						*alpha,
		const mlopenTensorDescriptor_t	xDesc,
		const void						*x,
		const void						*beta,
		const mlopenTensorDescriptor_t	 yDesc,
		void							*y);

/* This function adds the scaled values of a bias tensor to another tensor.
 * Each dimension of the bias tensor A must match the corresponding dimension
 * of the destination tensor C or must be equal to 1. In the latter case, the
 * same value from the bias tensor for those dimensions will be used to blend
 * into the C tensor.
 */
mlopenStatus_t mlopenAddTensor(mlopenHandle_t handle,
		const void						*alpha,
		const mlopenTensorDescriptor_t	aDesc,
		const void						*A,
		const void						*beta,
		const mlopenTensorDescriptor_t	 cDesc,
		void							*C);

/* This function implements the equation C = op ( alpha1[0] * A, alpha2[0] * B
 * ) + beta[0] * C, given tensors A, B, and C and scaling factors alpha1,
 * alpha2, and beta. The op to use is indicated by the descriptor opTensorDesc.
 * Currently-supported ops are listed by the mlopenOpTensorDescriptor_t enum.
 */
mlopenStatus_t mlopenOpTensor(mlopenHandle_t handle,
		const mlopenOpTensorDescriptor_t opTensorDesc,
		const void						*alpha1,
		const mlopenTensorDescriptor_t	aDesc,
		const void						*A,
		const void						*alpha2,
		const mlopenTensorDescriptor_t	bDesc,
		const void						*B,
		const void						*beta,
		const mlopenTensorDescriptor_t	cDesc,
		void							*C);

/* Pointer in Host memory to a single value. All elements of the y tensor will
 * be set to value[0]. The data type of the element in value[0] has to match
 * the data type of tensor y.
 */
mlopenStatus_t mlopenSetTensor(mlopenHandle_t                 handle,
		const mlopenTensorDescriptor_t yDesc,
		void                          *y,
		const void                    *valuePtr );

mlopenStatus_t mlopenScaleTensor(mlopenHandle_t                 handle,
		const mlopenTensorDescriptor_t yDesc,
		void                          *y,
		const void                    *alpha );

mlopenStatus_t mlopenCreateFilterDescriptor(mlopenFilterDescriptor_t *filterDesc);

mlopenStatus_t mlopenInitFilterDescriptor(mlopenFilterDescriptor_t filterDesc,
		mlopenDataType_t datatype,
		int k,
		int c,
		int h,
		int w);

mlopenStatus_t mlopenGetFilterDescriptor(mlopenFilterDescriptor_t filterDesc,
		mlopenDataType_t datatype,
		int *k,
		int *c,
		int *h,
		int *w);

// TODO: Add APIs for N-dimensional filter descriptors. Tensorflow uses them.
//

mlopenStatus_t mlopenDestroyFilterDescriptor(mlopenFilterDescriptor_t filterDesc);

mlopenStatus_t mlopenCreateConvolutionDescriptor(mlopenConvolutionDescriptor_t *convDesc);

mlopenStatus_t mlopenInitConvolutionDescriptor(mlopenConvolutionDescriptor_t convDesc,
		mlopenConvolutionMode_t mode,
		int pad_h,
		int pad_w,
		int u,
		int v,
		int upscalex,
		int upscaley);

mlopenStatus_t mlopenGetConvolutionDescriptor(mlopenConvolutionDescriptor_t convDesc,
		mlopenConvolutionMode_t *mode,
		int *pad_h,
		int *pad_w,
		int *u,
		int *v,
		int *upscalex,
		int *upscaley);

/* This function returns the dimensions of the resulting 4D tensor of a 2D
 * convolution, given the convolution descriptor, the input tensor descriptor
 * and the filter descriptor This function can help to setup the output tensor
 * and allocate the proper amount of memory prior to launch the actual
 * convolution.
 */
mlopenStatus_t mlopenGetConvolutionForwardOutputDim(mlopenConvolutionDescriptor_t convDesc,
		const mlopenTensorDescriptor_t inputTensorDesc,
		const mlopenFilterDescriptor_t filterDesc,
		int *n,
		int *c,
		int *h,
		int *w);
		

// TODO: Add APIs for N-dimensional filter descriptors. Tensorflow uses them.
//

mlopenStatus_t mlopenDestroyConvolutionDescriptor(mlopenConvolutionDescriptor_t convDesc);

/* This function attempts all MLOpen algorithms for mlopenConvolutionForward(),
 * and outputs performance metrics to a user- allocated array of
 * mlopenConvolutionFwdAlgoPerf_t. These metrics are written in sorted fashion
 * where the first element has the lowest compute time.
 */
mlopenStatus_t mlopenFindConvolutionForwardAlgorithm(mlopenHandle_t handle,
		const mlopenTensorDescriptor_t xDesc,
		const mlopenFilterDescriptor_t wDesc,
		const mlopenConvolutionDescriptor_t condDesc,
		const mlopenTensorDescriptor_t yDesc,
		const int requestAlgoCount,
		int *returnedAlgoCount,
		mlopenConvolutionFwdAlgoPerf_t *perfResults);

mlopenStatus_t mlopenConvolutionForward(mlopenHandle_t handle,
		const void *alpha,
		const mlopenTensorDescriptor_t xDesc,
		const void *x,
		const mlopenFilterDescriptor_t wDesc,
		const void *w,
		const mlopenConvolutionDescriptor_t convDesc,
		mlopenConvolutionFwdAlgo_t algo,
		const void *beta,
		const mlopenTensorDescriptor_t yDesc,
		void *y);
#endif
// TODO: Add APIs for finding the best convolution algorithm and performance, workspace metrics.
#ifdef __cplusplus
}
#endif
