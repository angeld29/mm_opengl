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

namespace angel{
		static const int entname_size = 0x20;

	bool    blvMap::DetectBLVVersion()
	{
		uint32_t		off = 0x88;
#define CHECK_OFF( xxx ) { /*angel::Log << angel::aeLog::debug <<boost::format( " off = %8x size = %8x")% off % (xxx) <<angel::aeLog::endl;*/\
	off += (xxx);if( off > datasize || off < 0 ) return false;}	

		{//Vertexs
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Vertex");
			map_data.num_vertex = *(int*)(data+off); off+=4;
			map_data.vertex_data = (short*)(data+off);
			CHECK_OFF( map_data.num_vertex*6 );
		}
		{//Faces
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Faces");
			map_data.num_faces = *(int*)(data+off);  off+=4;
			map_data.faces_array = data+off;
			CHECK_OFF( map_data.num_faces * map_sizes.facesize );
			map_data.facedatasize = *(int*)(data+0x68);
			map_data.facedata = (short*)( data+off );
			CHECK_OFF( map_data.facedatasize );

			map_data.facetextures = data + off;
			CHECK_OFF( map_data.num_faces *0x0a );

			map_data.numfaceparms = *(int*)(data+off);  off+=4;
			//faceparams1 = data +off;
			map_data.faceparams1  = (blv_faceparams_t*)(data+off);
			CHECK_OFF(map_data.numfaceparms *0x24);
			map_data.faceparams2 = data +off;
			CHECK_OFF(map_data.numfaceparms *0x0a);
		}
		{
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Sectors");
			map_data.num_sectors = *(int*)(data+off);off+=4;
			map_data.sectorsdata = data +off;
			CHECK_OFF(map_data.num_sectors*map_sizes.sectorsize);

			map_data.Rdatasize = *(int*)(data+0x6c);
			map_data.sectorsRdata = data + off;
			CHECK_OFF(map_data.Rdatasize);
			map_data.RLdatasize = *(int*)(data+0x70);
			map_data.sectorsRLdata = data + off;
			CHECK_OFF(map_data.RLdatasize);
		}
		{
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Sprites");
			map_data.num_sprites_hz = *(int*)(data+off); off+=4;
			map_data.num_sprites = *(int*)(data+off);off+=4;
			map_data.spritesdata = data + off;
			CHECK_OFF(map_data.num_sprites*map_sizes.spritesize);
			map_data.spritesnamesdata = (char*)(data + off);
			CHECK_OFF(0x20 * map_data.num_sprites);
		}
		{
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Lights");
			map_data.num_lights = *(int*)(data+off); off+=4;
			map_data.lightsdata = data + off;
			CHECK_OFF(map_data.num_lights*map_sizes.lightsize);
		}
		{
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Unk9");
			map_data.num_unk9 = *(int*)(data+off); off+=4;
			map_data.Unknown9data = data + off;
			CHECK_OFF(map_data.num_unk9*8);
		}
		{
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Spawn");
			map_data.num_spawn = *(int*)(data+off); off+=4;
			map_data.spawndata = data + off;
			CHECK_OFF(map_data.num_spawn*map_sizes.spawnsize);
		}
		{
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Outline");
			map_data.num_outline = *(int*)(data+off); off+=4;
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
    }
	blvMap::blvMap( pLodData loddata, std::string name):
        mmMap(loddata,name), data(&(*loddata)[0]),datasize(loddata->size())
	{
		angel::Log <<  "Load blvMap " <<  name << angel::aeLog::endl;
		if(!PrepareBLV() )
		{
            angel::Log <<  "Cannot detect blv version" <<  mapname << angel::aeLog::endl;
            return;
		};
        LoadBLVMap( );
		
		//angel::Log <<  "Loaded faces: " << faces.size() << ", vertexs: " << vertexes.size() << angel::aeLog::endl;
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

	void blvMap::Draw()
    {
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
