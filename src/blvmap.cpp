/*

Copyright (C) 2010 Angel (angel.d.death@gmail.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the included (GNU.txt) GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include "blvMap.h"
#include "zlib.h"
#include "log.h"
#include "aeTexture.h"
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/mesh.h>

namespace angel{
    Mesh* blvMap::CreateMesh(const face_t& face){
        vector<Vertex> vertices_list;
        vector<unsigned int> indices;
        vector<Texture> textures;
        if( face.blv_face.numvertex < 2 )
        {
            return NULL;
        }
        for(int i = 0; i < face.blv_face.numvertex+1; i++){
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            mm_short_vec3_s mm_vertex = vertexes[ face.vertex_idx[i] ];
            vector.x = (float)mm_vertex.x;
            vector.y = (float)mm_vertex.y;
            vector.z = (float)mm_vertex.z;
            //angel::Log << vector.x << " " << vector.y << " " << vector.z << angel::aeLog::endl;
            //angel::Log << mm_vertex.x << " " << mm_vertex.y << " " << mm_vertex.z << angel::aeLog::endl;
            vertex.Position = vector;
            // normals
            vector.x = (float)face.vertex_normal_x[i]/65536.0f;
            vector.y = (float)face.vertex_normal_y[i]/65536.0f;
            vector.z = (float)face.vertex_normal_z[i]/65536.0f;
            vertex.Normal = vector;
//            angel::Log << vector.x << " " << vector.y << " " << vector.z << angel::aeLog::endl;

            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = float( face.blv_faceparam.tex_u + face.vertex_tex_x[i] ) / face.tex->Width(); 
            vec.y = float( face.blv_faceparam.tex_v + face.vertex_tex_y[i] ) / face.tex->Height(); 
            vertex.TexCoords = vec;
            //angel::Log << vec.x << " " << vec.y << angel::aeLog::endl;
            vertices_list.push_back(vertex);
        }
        for(int i = 1; i < face.blv_face.numvertex - 1; i++){
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i+1);
        }
        angel::Log << indices.size() << angel::aeLog::endl;
        Texture texture;
        texture.id = face.tex->glID(); 
        texture.type = "texture_diffuse";
        texture.path = "texture";
        textures.push_back(texture);

        return new Mesh(vertices_list, indices, textures);
    }
    static const int entname_size = 0x20;

	bool    blvMap::DetectBLVVersion()
	{
		uint32_t		off = 0x88;
#define CHECK_OFF( xxx ) { /*angel::Log << angel::aeLog::debug <<boost::format( " off = %8x size = %8x")% off % (xxx) <<angel::aeLog::endl;*/\
	off += (xxx);if( off > datasize || off < 0 ) return false;}	

		{//Vertexs
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Vertex");
			map_data.num_vertex = *(uint32_t*)(data+off); off+=4;
			map_data.vertex_data = (int16_t*)(data+off);
			CHECK_OFF( map_data.num_vertex*6 );
		}
		{//Faces
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Faces");
			map_data.num_faces = *(uint32_t*)(data+off);  off+=4;
			map_data.faces_array = data+off;
			CHECK_OFF( map_data.num_faces * map_sizes.facesize );
			map_data.facedatasize = *(uint32_t*)(data+0x68);
			map_data.facedata = (int16_t*)( data+off );
			CHECK_OFF( map_data.facedatasize );

			map_data.facetextures = data + off;
			CHECK_OFF( map_data.num_faces *0x0a );

			map_data.numfaceparms = *(uint32_t*)(data+off);  off+=4;
			//faceparams1 = data +off;
			map_data.faceparams1  = (blv_faceparams_t*)(data+off);
			CHECK_OFF(map_data.numfaceparms *0x24);
			map_data.faceparams2 = (blv_faceparams2_t*)(data +off);
			CHECK_OFF(map_data.numfaceparms *0x0a);
		}
		{
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Sectors");
			map_data.num_sectors = *(uint32_t*)(data+off);off+=4;
			map_data.sectorsdata = data +off;
			CHECK_OFF(map_data.num_sectors*map_sizes.sectorsize);

			map_data.Rdatasize = *(uint32_t*)(data+0x6c);
			map_data.sectorsRdata = data + off;
			CHECK_OFF(map_data.Rdatasize);
			map_data.RLdatasize = *(uint32_t*)(data+0x70);
			map_data.sectorsRLdata = data + off;
			CHECK_OFF(map_data.RLdatasize);
		}
		{
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Sprites");
			map_data.num_sprites_hz = *(uint32_t*)(data+off); off+=4;
			map_data.num_sprites = *(uint32_t*)(data+off);off+=4;
			map_data.spritesdata = data + off;
			CHECK_OFF(map_data.num_sprites*map_sizes.spritesize);
			map_data.spritesnamesdata = (char*)(data + off);
			CHECK_OFF(0x20 * map_data.num_sprites);
		}
		{
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Lights");
			map_data.num_lights = *(uint32_t*)(data+off); off+=4;
			map_data.lightsdata = data + off;
			CHECK_OFF(map_data.num_lights*map_sizes.lightsize);
		}
		{
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Unk9");
			map_data.num_unk9 = *(uint32_t*)(data+off); off+=4;
			map_data.Unknown9data = data + off;
			CHECK_OFF(map_data.num_unk9*8);
		}
		{
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Spawn");
			map_data.num_spawn = *(uint32_t*)(data+off); off+=4;
			map_data.spawndata = data + off;
			CHECK_OFF(map_data.num_spawn*map_sizes.spawnsize);
		}
		{
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Outline");
			map_data.num_outline = *(uint32_t*)(data+off); off+=4;
			map_data.mapoutlinedata= data + off;
			CHECK_OFF(map_data.num_outline*0xc);
		}
		if( off != datasize )return false;
		return true;
	}
	bool    blvMap::PrepareBLV()
	{
		angel::Log.Print(angel::aeLog::LOG_DEBUG,"Try BLV6");
		version		= 	 6;
		map_sizes.facesize	= 0x50;  
		map_sizes.sectorsize	= 0x74;
		map_sizes.spritesize	= 0x1c;
		map_sizes.lightsize	= 0x0C; 
		map_sizes.spawnsize	= 0x14;
		if( DetectBLVVersion() ) return true;

		angel::Log.Print(angel::aeLog::LOG_DEBUG,"Try BLV7");
		version		= 	 7;
		map_sizes.facesize	= 0x60;  
		map_sizes.sectorsize	= 0x74;
		map_sizes.spritesize	= 0x20;
		map_sizes.lightsize	= 0x10; 
		map_sizes.spawnsize	= 0x18;
		if( DetectBLVVersion() ) return true;

		angel::Log.Print(angel::aeLog::LOG_DEBUG,"Try BLV8");
		version		= 	 8;
		map_sizes.facesize	= 0x60;  
		map_sizes.sectorsize	= 0x78;
		map_sizes.spritesize	= 0x20;
		map_sizes.lightsize	= 0x14; 
		map_sizes.spawnsize	= 0x18;
		if( DetectBLVVersion() ) return true;
		return false;
	}
    void	blvMap::LoadBLVMap() {
		{// get vertexs
			vertexes.resize(map_data.num_vertex);
			std::copy((mm_short_vec3_s*)map_data.vertex_data,(mm_short_vec3_s*)map_data.vertex_data+map_data.num_vertex,vertexes.begin());

		}
		{//faces
			faces.reserve( map_data.num_faces );
			int16_t  *v = ( int16_t * ) map_data.facedata;

            char texture_name[0xa+1];
            texture_name[0xa]=0;
			for ( int i = 0; i < map_data.num_faces; i++ )
			{

				face_t  face;// = faces[i];
				std::copy((char*)map_data.facetextures + i * 0xa,(char*)map_data.facetextures + i * 0xa+0x0a,texture_name);
                face.texname = texture_name;

				uint8_t* bindata = map_data.faces_array + i * map_sizes.facesize;
				if( version > 6) 
				{
					face.blv_plane7 = *(mm_float_plane_s*)(bindata);
					face.blv_face = *(blv_face_t*)(bindata +0x10);
				}else
				{
					face.blv_face = *(blv_face_t*)(bindata );
					face.blv_plane7.normal.x = (float)(face.blv_face.plane.normal.x/65536.0);
					face.blv_plane7.normal.y = (float)(face.blv_face.plane.normal.y/65536.0);
					face.blv_plane7.normal.z = (float)(face.blv_face.plane.normal.z/65536.0);
					face.blv_plane7.dist	 = (float)(face.blv_face.plane.dist/65536.0);
				}
				face.blv_faceparam = map_data.faceparams1[face.blv_face.fparm_index];
				face.blv_faceparam2 = map_data.faceparams2[face.blv_face.fparm_index];
				
				int numv = face.blv_face.numvertex;

				if( numv < 2 )
				{
					angel::Log << angel::aeLog::debug <<"Face " << i << " has  < 3 nodes "<< numv << " off:"<< bindata - data << angel::aeLog::endl;
				}

                face.vertex_idx.resize( numv +1);
				face.vertex_normal_x.resize( numv +1);
				face.vertex_normal_y.resize( numv +1);
				face.vertex_normal_z.resize( numv +1);
				face.vertex_tex_x.resize( numv + 1 );
				face.vertex_tex_y.resize( numv + 1 );
				
                memcpy(&face.vertex_idx[0], v + (numv+1)* 0, numv*2 +2 );
				memcpy(&face.vertex_normal_x[0], v + (numv+1)* 1, numv*2 +2 );
				memcpy(&face.vertex_normal_y[0], v + (numv+1)* 2, numv*2 +2);
				memcpy(&face.vertex_normal_z[0], v + (numv+1)* 3, numv*2+2);
				memcpy(&face.vertex_tex_x[0], v + (numv+1)* 4, numv*2+2);
				memcpy(&face.vertex_tex_y[0], v + (numv+1)* 5, numv*2+2);
				v += ( numv + 1 ) * 6;

                if( face.texname.size() )
                {
                    face.tex = TexManager.GetTexture( face.texname, TT_Texture);
                    face.mesh.reset ( CreateMesh( face ));
                }

				faces.push_back(face);
			}

		}
    }
	blvMap::blvMap( pLodData loddata, std::string name):
        mmMap(loddata,name), data(&(*loddata)[0]),datasize(loddata->size()),
        ourShader("resources/1.model_loading.vs", "resources/1.model_loading.fs")
	{
		angel::Log <<  "Load blvMap " <<  name << angel::aeLog::endl;
		if(!PrepareBLV() )
		{
            angel::Log <<  "Cannot detect blv version" <<  mapname << angel::aeLog::endl;
            return;
		};
        LoadBLVMap( );
		
		angel::Log <<  "Loaded faces: " << faces.size() << ", vertexs: " << vertexes.size() << angel::aeLog::endl;
	}
	
	void blvMap::TogglePortals()
	{
/*		uint32_t vismask=mSceneMgr->getVisibilityMask();

		if(!showportals)
			mSceneMgr->setVisibilityMask(vismask|PORTALS_MASK);
		else
			mSceneMgr->setVisibilityMask(vismask - (vismask&PORTALS_MASK));
		showportals=!showportals;*/
	}
	void blvMap::ToggleEnts()
	{
/*		uint32_t vismask=mSceneMgr->getVisibilityMask();
		DeselectAll();

		if(!showents)
			mSceneMgr->setVisibilityMask(vismask|(SPAWN_MASK|ENTITY_MASK));
		else
			mSceneMgr->setVisibilityMask(vismask - (vismask&(SPAWN_MASK|ENTITY_MASK)));
		showents=!showents;*/
	}

	void blvMap::Draw(glm::mat4 projection, glm::mat4 view, Camera camera)
    {
        
        ourShader.use();
        // pass projection matrix to shader (note that in this case it could change every frame)
        ourShader.setMat4("projection", projection);
        // camera/view transformation
        ourShader.setMat4("view", view);
        glm::mat4 model = glm::mat4();
        //model = glm::translate(model, glm::vec3(0.0f,0.0f,0.0f));
        //model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        ourShader.setMat4("model", model);
        
        int count = 0;
		for(  auto i  = faces.begin(); i != faces.end(); ++i)
        {
            face_t &face = *i;
            if( ! face.mesh ) continue;
            face.mesh->Draw(ourShader);
            count++;
        }
    }

	blvMap::~blvMap()
	{
/*		DeselectAll();
		if( data )
			delete[]data;

		vertexes.clear();
		faces.clear();
		//OverlayManager::getSingleton().getByName("MMView/MapOverlay")->hide();
		OverlayManager::getSingleton().getByName("MMView/blvMapFaceInfoOverlay")->hide();*/
	}

	void blvMap::Select()
	{
/*		Vector3 crosspoint;
		Real maxdist = 36000;
		uint32_t vismask=mSceneMgr->getVisibilityMask();

		DeselectAll();

		for( size_t i =0; i < faces.size(); i++)
		{
			face_t &face = faces[i];
			if( !showportals && isPortal(face) )
				continue;
			if(face.blvhdr.numvertex <3 )
				continue;
			static std::pair<bool, Real> result;
			result = ray.intersects(face.bbox);
			if(!result.first)
				continue;
			result = ray.intersects(face.plane);
			if(!result.first)
				continue;
			Real dist=result.second;
			//Real dist=face.plane.getDistance(ray.getOrigin());
			if( !isPortal(face) &&  face.plane.getDistance(ray.getOrigin())< 0)
				continue;
			crosspoint= ray.getPoint(dist);

			Vector3 v1=vertexes[face.vertex_idxs[0]]-crosspoint;
			Vector3 v2=vertexes[face.vertex_idxs[1]]-crosspoint;

			Vector3 basenormal( v1.crossProduct(v2));
			bool q = true;
			for( int j = 1 ; j < face.blvhdr.numvertex; j++ )
			{
				Vector3 v3=vertexes[face.vertex_idxs[j]]-crosspoint;
				Vector3 v4=vertexes[face.vertex_idxs[(j+1)%face.blvhdr.numvertex]]-crosspoint;
				if( basenormal.dotProduct(v3.crossProduct(v4)) < 0 )
				{
					q=false;
					break;
				}
			}
			if(!q)
				continue;
			if( dist > maxdist)
				continue;
			maxdist = dist;
			selected_face_index = (int)i;
		}
		if(vismask & ENTITY_MASK)
			for( size_t i =0; i < entites.size(); i++)
			{
				static std::pair<bool, Real> result;
				ent_t& ent=entites[i];
				//result = ray.intersects(Ogre::Sphere(Vector3(ent.mm6ent.origin),20));
				result = ray.intersects(ent.bbox);
				if(!result.first)
					continue;
				Real dist = (Ogre::Vector3(ent.mm6ent.origin)  - ray.getOrigin()).length();
				if( dist < maxdist )
				{
					DeselectAll();
					maxdist=dist;
					selected_ent_idx =(int) i;
				}
			}
			if(vismask & SPAWN_MASK)
				for( size_t i =0; i < spawns.size(); i++)
				{
					static std::pair<bool, Real> result;
					spawn_t& spw=spawns[i];
					//result = ray.intersects(Ogre::Sphere(Vector3(ent.mm6ent.origin),20));
					result = ray.intersects(spw.bbox);
					if(!result.first)
						continue;
					Real dist = result.second;
					if( dist < maxdist )
					{
						DeselectAll();
						maxdist=dist;
						selected_spawn_idx =(int) i;
					}
				}

				SelectFace();
				SelectEntity();
				SelectSpawn();*/
	}
}
