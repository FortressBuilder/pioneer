// Copyright © 2008-2016 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "attributes.glsl"
#include "logz.glsl"
#include "lib.glsl"

out vec2 uv;

void main(void)
{
	gl_Position = logarithmicTransform();
	gl_PointSize = a_normal.z;
	uv = a_normal.xy;
}
