#pragma once
#include "Camera.h"
#include "Light.h"
#include "GameInstance.h"

constexpr _uint_32 SHADOW_CASCADE_SIZE = 2048;
constexpr _uint_32 CASCADE_COUNT = 4;

std::pair<_matrix, _matrix> LightViewProjection_Cascades(weak_ptr<CLight> light, weak_ptr<CCamera> pCamera, _matrix const& projection__matrix, BoundingBox& cull_box)
{
	static float const far_factor = 1.5f;
	static float const light_distance_factor = 1.0f;

	BoundingFrustum frustum(projection__matrix);
	frustum.Transform(frustum, GET_SINGLE(CGameInstance)->Get_TransformMatrixInverse(CPipeLine::TS_VIEW));
	std::array<_float3, BoundingFrustum::CORNER_COUNT> corners{};
	frustum.GetCorners(corners.data());

	_float3 frustum_center(0, 0, 0);
	for (_float3 const& corner : corners)
	{
		frustum_center = frustum_center + corner;
	}
	frustum_center /= static_cast<float>(corners.size());

	float radius = 0.0f;
	for (_float3 const& corner : corners)
	{
		float distance = Distance(corner, frustum_center);
		radius = Max(radius, distance);
	}
	radius = std::ceil(radius * 8.0f) / 8.0f;

	_float3 const max_extents(radius, radius, radius);
	_float3 const min_extents = -max_extents;
	_float3 const cascade_extents = max_extents - min_extents;

	_float3 light_dir = XMVector3Normalize(XMLoadFloat4(&light.lock()->Get_LightDesc().vDirection));
	_matrix V = XMMatrixLookAtLH(frustum_center, frustum_center + light_distance_factor * light_dir * radius, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	float l = min_extents.x;
	float b = min_extents.y;
	float n = min_extents.z - far_factor * radius;
	float r = max_extents.x;
	float t = max_extents.y;
	float f = max_extents.z * far_factor;

	_float4x4 P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);
	_float4x4 VP = V * P;
	_float3 shadow_origin(0, 0, 0);
	shadow_origin = XMVector3TransformCoord(shadow_origin, VP);
	shadow_origin *= (SHADOW_CASCADE_SIZE / 2.0f);

	_float3 rounded_origin = XMVectorRound(shadow_origin);
	_float3 rounded_offset = rounded_origin - shadow_origin;
	rounded_offset *= (2.0f / SHADOW_CASCADE_SIZE);
	rounded_offset.z = 0.0f;
	P.m[3][0] += rounded_offset.x;
	P.m[3][1] += rounded_offset.y;

	BoundingBox::CreateFromPoints(cull_box, _float4(l, b, n, 1.0f), _float4(r, t, f, 1.0f));
	cull_box.Transform(cull_box, XMMatrixInverse(nullptr, V));
	return { V,P };
}




std::pair<_matrix, _matrix> LightViewProjection_Directional(weak_ptr<CLight> light, BoundingSphere const& scene_bounding_sphere, BoundingBox& cull_box)
{
	static const _float3 sphere_center = scene_bounding_sphere.Center;

	_float3 light_dir = XMVector3Normalize(XMLoadFloat4(&light.lock()->Get_LightDesc().vDirection));
	_float3 light_pos = -1.0f * scene_bounding_sphere.Radius * light_dir + sphere_center;
	_float3 target_pos = XMLoadFloat3(&scene_bounding_sphere.Center);
	_matrix V = XMMatrixLookAtLH(light_pos, target_pos, _float3(0.0f, 1.0f, 0.0f));
	_float3 sphere_centerLS = XMVector3TransformCoord(target_pos, V);

	float l = sphere_centerLS.x - scene_bounding_sphere.Radius;
	float b = sphere_centerLS.y - scene_bounding_sphere.Radius;
	float n = sphere_centerLS.z - scene_bounding_sphere.Radius;
	float r = sphere_centerLS.x + scene_bounding_sphere.Radius;
	float t = sphere_centerLS.y + scene_bounding_sphere.Radius;
	float f = sphere_centerLS.z + scene_bounding_sphere.Radius;

	_matrix P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	BoundingBox::CreateFromPoints(cull_box, _float4(l, b, n, 1.0f), _float4(r, t, f, 1.0f));
	cull_box.Transform(cull_box, XMMatrixInverse(nullptr, V));

	return { V,P };
}

std::array<_matrix, CASCADE_COUNT> RecalculateProjectionMatrices(weak_ptr<CCamera> pCamera, float split_lambda, std::array<float, CASCADE_COUNT>& split_distances)
{
	float Camera_near = pCamera.lock()->Get_Near();
	float Camera_far = pCamera.lock()->Get_Far() * 0.05f;
	float fov = pCamera.lock()->Get_Fovy();
	float ar = pCamera.lock()->Get_Aspect();
	float f = 1.0f / CASCADE_COUNT;
	for (_uint_32 i = 0; i < split_distances.size(); i++)
	{
		float fi = (i + 1) * f;
		float l = Camera_near * pow(Camera_far / Camera_near, fi);
		float u = Camera_near + (Camera_far - Camera_near) * fi;
		split_distances[i] = l * split_lambda + u * (1.0f - split_lambda);
	}

	std::array<_matrix, CASCADE_COUNT> projectionMatrices{};
	projectionMatrices[0] = XMMatrixPerspectiveFovLH(fov, ar, Camera_near, split_distances[0]);
	for (_uint_32 i = 1; i < projectionMatrices.size(); ++i)
		projectionMatrices[i] = XMMatrixPerspectiveFovLH(fov, ar, split_distances[i - 1], split_distances[i]);
	return projectionMatrices;
}


/**********************/


void LightViewProjection_Cascades2(_float4x4 ViewInvMatrix, weak_ptr<CCamera> pCamera, weak_ptr<CLight> light, _float4x4* shadowOrthoProj)
{
	// 카메라의 역행렬과 시야각,화면비, 가까운 평면 Z,먼 평면 Z
	_float4x4 camInv = ViewInvMatrix;
	float fov = pCamera.lock()->Get_Fovy();
	float ar = pCamera.lock()->Get_Aspect();
	float nearZ = pCamera.lock()->Get_Near();
	float farZ = pCamera.lock()->Get_Far();

	//시야각을 이용하여 수직 시야각을 구함
	float tanHalfVFov = tanf((fov / 2.0f));
	// 수직 시야각을 이용하여 수평 시야각을 구함
	float tanHalfHFov = tanHalfVFov * ar;

	_float cascadeEnd[CASCADE_COUNT + 1];
	// 절두체를 나누기위한 각 부분 절두체의 끝 지점 선언
	cascadeEnd[0] = nearZ;
	cascadeEnd[1] = 100.f;
	cascadeEnd[2] = 200.f;
	cascadeEnd[3] = 400.f;
	cascadeEnd[4] = farZ;
	// 3개의 절두체로 나누기위해 3번 반복함
	for (uint32_t i = 0; i < 4; ++i)
	{
		//+X,+Y 좌표에 수평,수직 시야각을 이용하여 구함. 각 부분 절두체의 가까운,먼
		//평면의 값을 곱하여 4개의 점을 구함
		float xn = cascadeEnd[i] * tanHalfHFov;
		float xf = cascadeEnd[i + 1] * tanHalfHFov;
		float yn = cascadeEnd[i] * tanHalfVFov;
		float yf = cascadeEnd[i + 1] * tanHalfVFov;
		//+좌표값을 구하면 -좌표값을 구하여 각각의 절두체 평면을 구할수있음.
		//각 절두체의 Z값을 저장하여 i 가 낮은 순서로 가까운 평면 먼평면을 구성함
		_float4 frustumCorners[8] =
		{
			//near Face
			{xn,yn,cascadeEnd[i],1.0f},
			{-xn,yn,cascadeEnd[i],1.0f},
			{xn,-yn,cascadeEnd[i],1.0f},
			{-xn,-yn,cascadeEnd[i],1.0f},
			//far Face
			{xf,yf,cascadeEnd[i + 1],1.0f},
			{-xf,yf,cascadeEnd[i + 1],1.0f},
			{xf,-yf,cascadeEnd[i + 1],1.0f},
			{-xf,-yf,cascadeEnd[i + 1],1.0f}
		};

		_float4 centerPos = _float4(0.0f, 0.f, 0.f, 0.f);
		for (uint32_t j = 0; j < 8; ++j)
		{
			frustumCorners[j] = XMVector3TransformCoord(frustumCorners[j], camInv);
			centerPos += frustumCorners[j];
		}
		centerPos /= 8.0f;
		float radius = 0.0f;
		for (uint32_t j = 0; j < 8; ++j)
		{
			float distance = XMVectorGetX(XMVector3Length(frustumCorners[j] - centerPos));
			radius = Max(radius, distance);
		}

		radius = std::ceil(radius * 16.0f) / 16.0f;

		// using radius ,  we made aabb box
		_float3 maxExtents = _float3(radius, radius, radius);
		_float3 minExtents = -maxExtents;

		_float3 lightDirection = XMVector3Normalize(XMLoadFloat4(&light.lock()->Get_LightDesc().vDirection));
		_float3 shadowCamPos = _float3(centerPos) + (XMVector3Normalize(lightDirection) * (minExtents.z));
		_float4x4 lightMatrix = XMMatrixLookAtLH(shadowCamPos, _float3(centerPos), _float3(0.0f, 1.0f, 0.0f));
		_float3 cascadeExtents = maxExtents - minExtents;
		shadowOrthoProj[i] = XMMatrixOrthographicOffCenterLH(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, cascadeExtents.z) * lightMatrix;
	}
}