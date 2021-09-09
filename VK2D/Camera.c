/// \file Camera.c
/// \author Paolo Mazzon
#include "VK2D/Camera.h"
#include "VK2D/Renderer.h"
#include "VK2D/Buffer.h"
#include "VK2D/Validation.h"

VK2DCameraIndex vk2dCameraCreate(VK2DCameraSpec spec) {
	// TODO: Check renderer's list of cameras for an available spot and place the camera there, creating necessary UBOs as well
	VK2DRenderer gRenderer = vk2dRendererGetPointer();
	VK2DCameraIndex position = VK2D_INVALID_CAMERA;

	// Find a spot for a new camera
	for (int i = 0; i < VK2D_MAX_CAMERAS && position == VK2D_INVALID_CAMERA; i++)
		if (gRenderer->cameras[i].state == cs_Deleted)
			position = i;

	// Create the new camera
	if (position != VK2D_INVALID_CAMERA) {
		// Setup pointer and basic info
		VK2DCamera *cam = &gRenderer->cameras[position];
		cam->spec = spec;
		cam->state = cs_Normal;

		// Create the lists first
		cam->ubos = calloc(1, sizeof(VK2DUniformBufferObject) * gRenderer->swapchainImageCount);
		cam->buffers = malloc(sizeof(VK2DBuffer) * gRenderer->swapchainImageCount);
		cam->uboSets = malloc(sizeof(VkDescriptorSet) * gRenderer->swapchainImageCount);
		vk2dPointerCheck(cam->ubos);
		vk2dPointerCheck(cam->buffers);
		vk2dPointerCheck(cam->uboSets);

		// Populate the lists TODO: This
		/*for (i = 0; i < gRenderer->swapchainImageCount; i++) {
			gRenderer->uboBuffers[i] = vk2dBufferCreate(gRenderer->ld, sizeof(VK2DUniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			_vk2dCameraUpdateUBO(&gRenderer->ubos[i], &gRenderer->camera);
			_vk2dRendererFlushUBOBuffer(i);
			gRenderer->uboSets[i] = vk2dDescConGetBufferSet(gRenderer->descConVP, gRenderer->uboBuffers[i]);
		}*/
	}

	return position;
}

void vk2dCameraUpdate(VK2DCameraIndex index, VK2DCameraSpec spec) {
	VK2DRenderer gRenderer = vk2dRendererGetPointer();
	memcpy(&gRenderer->cameras[index].spec, &spec, sizeof(VK2DCameraSpec));
}

void vk2dCameraSetState(VK2DCameraIndex index, VK2DCameraState state) {
	VK2DRenderer gRenderer = vk2dRendererGetPointer();
	if (state == cs_Deleted || state == cs_Reset) { // Free internal resources
		for (int i = 0; i < gRenderer->swapchainImageCount; i++)
			vk2dBufferFree(gRenderer->cameras[index].buffers[i]);
		free(gRenderer->cameras[index].ubos);
		free(gRenderer->cameras[index].buffers);
		free(gRenderer->cameras[index].uboSets);
	}
	gRenderer->cameras[index].state = state;
}

VK2DCameraState vk2dCameraGetState(VK2DCameraIndex index) {
	VK2DRenderer gRenderer = vk2dRendererGetPointer();
	return gRenderer->cameras[index].state;
}
