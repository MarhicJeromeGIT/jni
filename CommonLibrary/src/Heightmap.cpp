#include "Heightmap.h"

using namespace glm;

Heightmap::Heightmap( const char* filename, const vec3& pos, const vec3& scl ) : position(pos), scale(scl)
{
	model = new Model();
	model->modelName = filename;

	// create a MODEL from a heightmap:
	int bpp = 0;
	bool alpha = false;
	unsigned char* datac = readTGA( filename, width, height, bpp, alpha);

	vec3 scaleFactor = vec3( scale.x / (float)width, scale.y / 255.0f, scale.z / (float) height );

	Model::Mesh mesh = Model::Mesh();

	heightdata = new vec3*[width];
	normales = new vec3[width*height];
	for( int i=0; i<width; i++ )
	{
		heightdata[i] = new vec3[height];
	}

	mesh.nbVertices = width * height;
	mesh.vertex_positions = new vec3[mesh.nbVertices];
	mesh.vertex_normales = new vec3[mesh.nbVertices];
	mesh.vertex_tangentes = new vec3[mesh.nbVertices];
	mesh.vertex_uvs = new vec2[mesh.nbVertices];
	mesh.hasSecondUVSet = true;
	mesh.vertex_uvs2 = new vec2[mesh.nbVertices];

	maxHeight = 40.0f;
	float heightScale = maxHeight/255.0f;
	for( int j=0; j< height; j++ )
	{
		for( int i=0; i< width; i++ )
		{
			heightdata[i][j] = vec3( (float)i * scaleFactor.x , datac[ 3 * (i + (height-1-j)*width) ] * heightScale * scaleFactor.y, (float)j * scaleFactor.z );
			heightdata[i][j] += position;
		}
	}
	maxHeight = 40.0f + position.y;

	int k=0;
	for( int j=0; j< height; j++ )
	{
		for( int i=0; i< width; i++ )
		{
			// y up
			mesh.vertex_positions[k]  = heightdata[i][j];
			mesh.vertex_normales[k]   = vec3(0,1,0);
			mesh.vertex_tangentes[k]  = vec3(1,0,0);
			mesh.vertex_uvs[k]        = vec2( (float)i/(float)width, 1.0-(float)j/(float)height);

			float u = (float)i/(float)width;
			float v = (float)j/(float)height;
			u = 10.0*u;
			v = 10.0*v;
			double intpart = 0;
			u = modf (u, &intpart);

			v = modf (v, &intpart);
			mesh.vertex_uvs2[k]       = vec2( i%2 , (j%2) * 0.333f );
			k++;
		}
	}

	for( int j=1; j< height-1; j++ )
	{
		for( int i=1; i< width-1; i++ )
		{
			vec3 diffX = normalize( heightdata[i+1][j] - heightdata[i-1][j] );
			vec3 diffY = normalize( heightdata[i][j-1] - heightdata[i][j+1] );
			vec3 normale = normalize( cross( diffX, diffY ) );

			k = i + j*width;
			mesh.vertex_normales[k]   = normale;
			mesh.vertex_tangentes[k]  = normalize(diffX);

			normales[k] = normale;

			k++;
		}
	}

	// triangles :
	k = 0;
	mesh.nbTriangles = (height-1)*(width-1)*2;
	mesh.indices = new ivec3[mesh.nbTriangles];
	for( int j=0; j< height-1; j++ )
	{
		for( int i=0; i< width-1; i++ )
		{
			// y up
			mesh.indices[k] = ivec3(i+j*width, i+(j+1)*width, (i+1)+j*width);
			mesh.indices[k+1] = ivec3(i+(j+1)*width, (i+1)+j*width, (i+1) + (j+1)*width);
			k += 2;
		}
	}

	mesh.hasBones = false;
	mesh.materialIndex = 0;
	model->meshes.push_back(mesh);

	// create a material to go with it :
	Model::Material material;
	material.diffuse_texture = "colormap.tga";
	material.hasDiffuseTexture = true;
	material.hasNormalTexture = false;
	model->materials.push_back( material );

}

void Heightmap::draw()
{
/*	glBegin(GL_TRIANGLES);
	for( int j=0; j< height-1; j++ )
	{
		for( int i=0; i< width-1; i++ )
		{
			vec3 p0 = vec3( i, heightData[i + j*width ] ,j);
			vec3 p1 = vec3( i+1, heightData[i+1 + j*width ] ,j);
			vec3 p2 = vec3( i, heightData[i + (j+1)*width ] ,j+1);
			vec3 p3 = vec3( i+1, heightData[i+1 + (j+1)*width ] ,j+1);

			glColor3f( (float)i/255.0f, (float)j/255.0f, 1);
			glVertex3f(p0.x,p0.y,p0.z);
			glVertex3f(p1.x,p1.y,p1.z);
			glVertex3f(p2.x,p2.y,p2.z);

			glVertex3f(p1.x,p1.y,p1.z);
			glVertex3f(p2.x,p2.y,p2.z);
			glVertex3f(p3.x,p3.y,p3.z);
			// = datac[ i + j*width ];
		}
	}
	glEnd();
	*/
}

// donne la hauteur y d'un point x,z sur la carte :
float Heightmap::getHeight( float x, float z)
{
/*	// pas de scaling pour le moment :
	if( x < 0 || x > dimY-2 || z < 0 || z > dimX-2 )
		return 0.0f;

	int xt = (int) z;
	int yt = (int) x;
	float resteX = z-(float)xt;
	float resteY = x-(float)yt;
	// lequel des deux triangles ?
	vec3 p0,p1,p2;
	if( resteX + resteY < 1.0f )
	{
		p0 = posData[ xt* dimY + yt ];
		p1 = posData[ (xt+1)*dimY + yt ];
		p2 = posData[ xt * dimY + yt + 1];
	}
	else
	{
		p0 = posData[ xt*dimY + yt + 1];
		p1 = posData[ (xt+1)*dimY + yt ];
		p2 = posData[ (xt+1) * dimY + yt + 1];
	}
	// on a un plan :
	Plane p(p0,p1,p2);
	// resout l'equation pour y tq Ax+By+Cz+D=0
	float A = p.ABC.x;
	float B = p.ABC.y;
	float C = p.ABC.z;
	float y = p0.y; //float y = (p0.y+p1.y+p2.y)/3.0f; return y;
	if( B != 0.0 ) // mur vertical ?
	{
		y = -(A*x + C*z + p.D) / B;
	}
	return y;*/
	return 0;
}


Terrain::Terrain(Heightmap* map, const mat4& mat, OpenGLModel* glModel, const Model* model ) : OpenGLStaticModelInstance(mat,glModel,model), heightmap(map)
{
/*	TextureGL* detail = TextureManager::get()->loadTexture(  "details.tga", "details.tga" );

	for( int meshIndex = 0; meshIndex < internModel->meshes.size(); meshIndex++ )
	{
		delete materials[meshIndex];
		MaterialMultiTexture* newMat = new MaterialMultiTexture(2);
		newMat->setTextureID(0, glModel->meshes[meshIndex].texture->getTexId());
		newMat->setTextureID(1, detail->getTexId());
		materials[meshIndex] = newMat;
	}

	vec3* weights = new vec3[ glModel->meshes[0].totalVertices ];

	int k=0;
	for( int j=0; j< heightmap->height; j++ )
	{
		for( int i=0; i<  heightmap->width; i++ )
		{
			float height = heightmap->heightdata[i][j].y / heightmap->maxHeight;
			float normaleCoeff = heightmap->normales[k].y;
			weights[k] = normalize( vec3(normaleCoeff,height,1.0-normaleCoeff) );

			k++;
		}
	}

	glGenBuffers(1,&glModel->meshes[0].textureWeightBuffer);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, glModel->meshes[0].textureWeightBuffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(vec3) * glModel->meshes[0].totalVertices, weights, GL_STATIC_DRAW );
	*/
}

bool Terrain::getIntersection( const vec3& rayStart, const vec3& rayDir, vec3& intersectionPoint )
{
	// find the closest intersection point:
	bool intersect = false;

//	cout<<"raydir : "<<rayDir.x<<","<<rayDir.y<<","<<rayDir.z<<endl;

	// find imin such that
	// rayStar + rayDir * imin = maxHeight  and
	// rayStar + rayDir * imax = 0
	//if( abs(rayDir.y) < 0.01 )
	//	return false;

	// for which i can we first intersect a mountain ?
	//float imin = (heightmap->maxHeight -rayStart.y)/rayStart.y;
	// for which i will we be at 0 ?
	//float imax = (0.0 - rayStart.y) / rayDir.y;
	float imin = 0.0;
	float imax = 500.0f;
	float nbSteps = 300.0f;

//	cout<<"camera posy = "<<rayStart.y<<endl;
//	cout<<"imin,imax : "<<imin<<","<<imax<<endl;

	if( imin > imax )
	{
		float temp = imax;
		imax = imin;
		imin = imax;
	}
	float sizeStep = (imax-imin)/nbSteps;

	for( float i=imin; i< imax; i+= sizeStep )
	{
		vec3 pos0 = rayStart + rayDir * i;
		vec3 pos = pos0 - heightmap->position;

		if( pos.x < 0 || pos.x > heightmap->width-1 || pos.z < 0 || pos.z > heightmap->height-1 )
			continue;

		// get the height at this point (x,y) of the map:
		int coordX = pos.x;
		int coordZ = pos.z;

		float height = heightmap->heightdata[coordX][coordZ].y;
		if( height > pos0.y )
		{
			intersectionPoint = pos0;
			intersectionPoint.y = height;
			intersect = true;
			break;
		}
	}

	return intersect;
}


void Terrain::Draw(MATERIAL_DRAW_PASS Pass)
{
	if( Pass == MATERIAL_DRAW_PASS::SHADOW )
		return;

	OpenGLStaticModelInstance::Draw(Pass);

	/*
	// draw the normals
	glColor3f(1,0,0);
	glBegin(GL_LINES);
	for( int i=0; i< heightmap->width*heightmap->height; i++ )
	{
		vec3 pos = heightmap->model->meshes[0].vertex_positions[i];
		vec3 norm = pos + heightmap->normales[i];
		glVertex3f( pos.x, pos.y, pos.z );
		glVertex3f( norm.x, norm.y, norm.z );
	}
	glEnd();
	*/
}








