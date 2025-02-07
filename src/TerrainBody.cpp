// Copyright © 2008-2016 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "TerrainBody.h"
#include "GasGiant.h"
#include "GeoSphere.h"
#include "Pi.h"
#include "WorldView.h"
#include "Frame.h"
#include "Game.h"
#include "graphics/Graphics.h"
#include "graphics/Renderer.h"

TerrainBody::TerrainBody(SystemBody *sbody) :
	Body(),
	m_sbody(sbody),
	m_mass(0)
{
	InitTerrainBody();
}

TerrainBody::TerrainBody() :
	Body(),
	m_sbody(0),
	m_mass(0)
{
}

TerrainBody::~TerrainBody()
{
	m_baseSphere.reset();
}

void TerrainBody::InitTerrainBody()
{
	assert(m_sbody);
	m_mass = m_sbody->GetMass();
	if (!m_baseSphere) {
		if ( SystemBody::SUPERTYPE_GAS_GIANT==m_sbody->GetSuperType() ) {
			m_baseSphere.reset(new GasGiant(m_sbody));
		} else {
			m_baseSphere.reset(new GeoSphere(m_sbody));
		}
	}
	m_maxFeatureHeight = (m_baseSphere->GetMaxFeatureHeight() + 1.0) * m_sbody->GetRadius();
}

void TerrainBody::SaveToJson(Json::Value &jsonObj, Space *space)
{
	Body::SaveToJson(jsonObj, space);

	Json::Value terrainBodyObj(Json::objectValue); // Create JSON object to contain terrain body data.

	terrainBodyObj["index_for_system_body"] = space->GetIndexForSystemBody(m_sbody);

	jsonObj["terrain_body"] = terrainBodyObj; // Add terrain body object to supplied object.
}

void TerrainBody::LoadFromJson(const Json::Value &jsonObj, Space *space)
{
	Body::LoadFromJson(jsonObj, space);

	if (!jsonObj.isMember("terrain_body")) throw SavedGameCorruptException();
	Json::Value terrainBodyObj = jsonObj["terrain_body"];

	if (!terrainBodyObj.isMember("index_for_system_body")) throw SavedGameCorruptException();

	m_sbody = space->GetSystemBodyByIndex(terrainBodyObj["index_for_system_body"].asInt());
	InitTerrainBody();
}

void TerrainBody::Render(Graphics::Renderer *renderer, const Camera *camera, const vector3d &viewCoords, const matrix4x4d &viewTransform)
{
	matrix4x4d ftran = viewTransform;
	vector3d fpos = viewCoords;
	double rad = m_sbody->GetRadius();

	float znear, zfar;
	renderer->GetNearFarRange(znear, zfar);

	vector3d campos = fpos;
	ftran.ClearToRotOnly();
	campos = ftran.Inverse() * campos;

	campos = campos * (1.0/rad);		// position of camera relative to planet "model"

	std::vector<Camera::Shadow> shadows;
	if( camera ) {
		camera->PrincipalShadows(this, 3, shadows);
		for (std::vector<Camera::Shadow>::iterator it = shadows.begin(), itEnd=shadows.end(); it!=itEnd; ++it) {
			it->centre = ftran * it->centre;
		}
	}

	ftran.Scale(rad, rad, rad);

	// translation not applied until patch render to fix jitter
	m_baseSphere->Render(renderer, ftran, -campos, m_sbody->GetRadius(), shadows);

	ftran.Translate(campos.x, campos.y, campos.z);
	SubRender(renderer, ftran, campos);
}

void TerrainBody::SetFrame(Frame *f)
{
	if (GetFrame()) {
		GetFrame()->SetPlanetGeom(0, 0);
	}
	Body::SetFrame(f);
	if (f) {
		GetFrame()->SetPlanetGeom(0, 0);
	}
}

double TerrainBody::GetTerrainHeight(const vector3d &pos_) const
{
	double radius = m_sbody->GetRadius();
	if (m_baseSphere) {
		return radius * (1.0 + m_baseSphere->GetHeight(pos_));
	} else {
		assert(0);
		return radius;
	}
}

bool TerrainBody::IsSuperType(SystemBody::BodySuperType t) const
{
	if (!m_sbody) return false;
	else return m_sbody->GetSuperType() == t;
}

//static 
void TerrainBody::OnChangeDetailLevel()
{
	GeoSphere::OnChangeDetailLevel();
	GasGiant::OnChangeDetailLevel();
}
