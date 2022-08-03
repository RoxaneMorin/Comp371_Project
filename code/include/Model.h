//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs

#include <glm/glm.hpp>

using namespace glm;

class Model
{
public:
	Model();
	Model(vec3 position, vec3 rotation, vec3 scale);
	Model(vec3 position, vec3 rotation, vec3 scale, mat4 parent);
	virtual ~Model();

	virtual void Update(float dt) = 0;
	virtual void Draw(int shaderProgram, GLenum renderingMode = GL_TRIANGLES) = 0;

	virtual mat4 GetWorldMatrix() const;

	virtual void SetWorldMatrix(vec3 position, vec3 rotation, vec3 scale);

	void ResetParent();
	void SetParent(mat4 parentMatrix);
	void SetPosition(vec3 position);
	void SetScaling(vec3 scaling);
	void SetRotation(vec3 rotation);

	vec3 GetPosition() const { return mPosition; }
	vec3 GetScaling() const { return mScaling; }
	vec3 GetRotation() const { return mRotation; }

	virtual bool ContainsPoint(vec3 position) = 0;//Whether or not the given point is withing the model. For collisions.
	virtual bool IntersectsPlane(vec3 planePoint, vec3 planeNormal) = 0;
	//virtual float IntersectsRay(vec3 rayOrigin, vec3 rayDirection) = 0; //Returns a strictly positive value if an intersection occurs

	virtual bool isSphere() = 0; //This is not at all object-oriented, but somewhat necessary due to need for a simple double-dispatch mechanism

	// The vertex format could be different for different types of models
	struct TexturedColoredNormalVertex
	{
		vec3 position;
		vec3 color;
		vec2 uv;
		vec3 normals;

		// Constructors.
		TexturedColoredNormalVertex() : position(vec3(0.0f, 0.0f, 0.0f)), color(vec3(1.0f, 1.0f, 1.0f)), uv(vec2(0.0f, 0.0f)), normals(vec3(0.0f, 0.0f, 0.0f)) {}

		TexturedColoredNormalVertex(vec3 _position, vec3 _color, vec2 _uv, vec3 _normals) : position(_position), color(_color), uv(_uv), normals(_normals) {}

		// Copy constructor.
		TexturedColoredNormalVertex(TexturedColoredNormalVertex source, vec3 newNormals) : position(source.position), color(source.color), uv(source.uv), normals(newNormals) {}
	};

protected:
	vec3 mPosition;
	vec3 mScaling;
	vec3 mRotation;

	mat4 mMatrix;
	mat4 mParent;
};


