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
#include "mmMap.h"
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
			num_vertex = *(int*)(data+off); off+=4;
			vertex_data = (short*)(data+off);
			CHECK_OFF( num_vertex*6 );
		}
		{//Faces
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Faces");
			num_faces = *(int*)(data+off);  off+=4;
			faces_array = data+off;
			CHECK_OFF( num_faces * facesize );
			facedatasize = *(int*)(data+0x68);
			facedata = (short*)( data+off );
			CHECK_OFF( facedatasize );

			facetextures = data + off;
			CHECK_OFF( num_faces *0x0a );

			numfaceparms = *(int*)(data+off);  off+=4;
			//faceparams1 = data +off;
			faceparams1  = (faceparams_blv_t*)(data+off);
			CHECK_OFF(numfaceparms *0x24);
			faceparams2 = data +off;
			CHECK_OFF(numfaceparms *0x0a);
		}
		{
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Sectors");
			num_sectors = *(int*)(data+off);off+=4;
			sectorsdata = data +off;
			CHECK_OFF(num_sectors*sectorsize);

			Rdatasize = *(int*)(data+0x6c);
			sectorsRdata = data + off;
			CHECK_OFF(Rdatasize);
			RLdatasize = *(int*)(data+0x70);
			sectorsRLdata = data + off;
			CHECK_OFF(RLdatasize);
		}
		{
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Sprites");
			num_sprites_hz = *(int*)(data+off); off+=4;
			num_sprites = *(int*)(data+off);off+=4;
			spritesdata = data + off;
			CHECK_OFF(num_sprites*spritesize);
			spritesnamesdata = (char*)(data + off);
			CHECK_OFF(0x20 * num_sprites);
		}
		{
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Lights");
			num_lights = *(int*)(data+off); off+=4;
			lightsdata = data + off;
			CHECK_OFF(num_lights*lightsize);
		}
		{
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Unk9");
			num_unk9 = *(int*)(data+off); off+=4;
			Unknown9data = data + off;
			CHECK_OFF(num_unk9*8);
		}
		{
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Spawn");
			num_spawn = *(int*)(data+off); off+=4;
			spawndata = data + off;
			CHECK_OFF(num_spawn*spawnsize);
		}
		{
			angel::Log.Print(angel::aeLog::LOG_DEBUG,"Outline");
			num_outline = *(int*)(data+off); off+=4;
			mapoutlinedata= data + off;
			CHECK_OFF(num_outline*0xc);
		}
		if( off != datasize )return false;
		return true;
	}
	bool    blvMap::PrepareBLV()
	{
		angel::Log.Print(angel::aeLog::LOG_DEBUG,"Try BLV6");
		version		= 	 6;
		facesize	= 0x50;  
		sectorsize	= 0x74;
		spritesize	= 0x1c;
		lightsize	= 0x0C; 
		spawnsize	= 0x14;
		if( DetectBLVVersion() ) return true;

		angel::Log.Print(angel::aeLog::LOG_DEBUG,"Try BLV7");
		version		= 	 7;
		facesize	= 0x60;  
		sectorsize	= 0x74;
		spritesize	= 0x20;
		lightsize	= 0x10; 
		spawnsize	= 0x18;
		if( DetectBLVVersion() ) return true;

		angel::Log.Print(angel::aeLog::LOG_DEBUG,"Try BLV8");
		version		= 	 8;
		facesize	= 0x60;  
		sectorsize	= 0x78;
		spritesize	= 0x20;
		lightsize	= 0x14; 
		spawnsize	= 0x18;
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
/*	void blvMap::DeselectAll()
	{
		DeselectFace();
		DeselectSpawn();
		DeselectEntity();
	}
	void blvMap::SelectFace()
	{
		//SetblvMapFaceInfoOverlay();
		if( selected_face_index== -1)
		{
			DeselectFace();
			return;
		}
		face_t &face = faces[selected_face_index];

		//angel::Log << "select face " << selected_face_index << " "<< face.texture_name <<angel::aeLog::endl;

		SceneNode *Node;
		if( mSceneMgr->hasSceneNode("SelFaceNode"))
			Node = mSceneMgr->getSceneNode("SelFaceNode");
		else
			Node =  mSceneMgr->getRootSceneNode()->createChildSceneNode("SelFaceNode");


		ManualObject* manOb = mSceneMgr->createManualObject("SelFace");

		manOb->setDynamic(true);
		if(isPortal(face))
		{
			manOb->begin("BLV/SelectedPortal", RenderOperation::OT_TRIANGLE_LIST);
			manOb->setVisibilityFlags(PORTALS_MASK);
		}
		else
		{
			manOb->setVisibilityFlags(WALLS_MASK);
			manOb->begin("BLV/SelectedFace", RenderOperation::OT_TRIANGLE_LIST);
		}

		for( int j = 1 ; j < face.blvhdr.numvertex -1; j++ )
		{
			manOb->position(vertexes[face.vertex_idxs[0]]);
			manOb->position(vertexes[face.vertex_idxs[j]]);
			manOb->position(vertexes[face.vertex_idxs[j+1]]);
		}
		manOb->end();
		Node->attachObject(manOb);
		face_blv_t			facehdr = face.blvhdr;
		faceparams_blv_t faceparams = face.blvfaceparam;
		Overlay*mMyOverlay = OverlayManager::getSingleton().getByName("MMView/blvMapFaceInfoOverlay");
		mMyOverlay->show();
		std::stringstream s;
		s << "Face#: "<<selected_face_index <<"\t";
		s<< "Texture: " <<face.texture_name;
		s << "\tAttr:"<<std::hex << std::setw(8)<<std::setfill('0')<< facehdr.attrib <<"\n";
		s << std::dec ;
		s<< "BBox max:\t" <<face.bbox.getMinimum() << "\nBBox min:\t" << face.bbox.getMaximum() <<"\n";
		s<< "Plane:\t"<<face.plane.normal << "\nPlane dist:\t"<< face.plane.d <<"\n";
		//s<< "PlaneI:\t"<<facehdr.plane<<"\n";
		s<< "Plane7:\t"<<face.plane7<<"\n";
		s << "Z_CALC: ";
		s << facehdr.z_calc[0] <<"\t"<< facehdr.z_calc[1] <<"\t"<< facehdr.z_calc[2] <<"\t";
		s << facehdr.z_calc[3] <<"\t"<< facehdr.z_calc[4] <<"\t"<< facehdr.z_calc[5] <<"\n";


		s <<"FacetType: " <<std::hex << std::setw(2)<<std::setfill('0')<< (uint32_t)facehdr.facet_type<<"\t";
		s << std::dec ;
		s << "BitmapIdx: "<<facehdr.bitmap_index <<"\n";

		s << "Room: "<<facehdr.room_number <<"\tRoom behind: " <<facehdr.room_behind_number;
		s <<"\tPadding: "<< (int)facehdr.padding[0] <<"\t" << (int)facehdr.padding[1] <<"\n";
		s << "Fparm idx: "<<facehdr.fparm_index;
		s << "\tFacetIdx: "<<faceparams.facet_index;
		s << "\tBitmapIdx: "<< faceparams.bitmap_index <<"\n";

		s << "Fade X: "<< faceparams.fade.x << "\tY: "<< faceparams.fade.y << "\tZ: "<< faceparams.fade.z <<"\n";
		s << "Fade base X: "<< faceparams.fade_base_x << "\tY: " << faceparams.fade_base_y << "\n";
		s << "TextureFrameTbl idx: "<< faceparams.texture_frame_table_index;
		s << "\tCOG: " <<faceparams.texture_frame_table_COG <<"\n";
		s <<"#COG: "<< faceparams.cog_number;
		s <<"\t#EVT: "<< faceparams.cog_triggered;
		s << "\tType: "<< std::hex << std::setw(4)<<std::setfill('0')<< faceparams.cog_trigger_type <<"\n";
		s << std::dec ;
		s << "Light level: "<< faceparams.light_level <<"\n";
		OverlayManager::getSingleton().getOverlayElement("MMView/MMblvMap/FaceParamData")->setCaption(s.str());

	}
	void blvMap::DeselectFace()
	{
		Overlay*mMyOverlay = OverlayManager::getSingleton().getByName("MMView/blvMapFaceInfoOverlay");
		mMyOverlay->hide();
		mSceneMgr->destroyManualObject("SelFace");
		selected_face_index=-1;
	}
	void    blvMap::SelectEntity()
	{
		if( selected_ent_idx == -1 )
			return;
		Entity* ent_o = mSceneMgr->getEntity("Entity" + StringConverter::toString(selected_ent_idx));
		if( !ent_o )
		{
			angel::Log << "could not find entity " << selected_ent_idx <<angel::aeLog::endl;
		}else
		{
			ent_o->getParentSceneNode()->showBoundingBox(true);
		}
		Overlay*mMyOverlay = OverlayManager::getSingleton().getByName("MMView/ODMMapEntityInfoOverlay");
		mMyOverlay->show();
		std::stringstream s;
		ent_t& ent=entites[selected_ent_idx];
		s << "Entity#: "<< selected_ent_idx << "\t";
		s << "Name: "<< ent.name << "\n";
		s << "Origin: "<< ent.mm6ent.origin << "\n";

		s << "DecListId#: "<< ent.mm6ent.declist_id<< "\t";
		s << "AIattrMarkers: "<< ent.mm6ent.AIattrMarkers<< "\n";
		s << "Facing: "<< std::setw(4)<<std::setfill('0')  << ent.mm6ent.facing<< "\n";
		s << std::dec;

		s << "Evt1: "<< ent.mm6ent.evt1<< "\t";
		s << "Evt2: "<< ent.mm6ent.evt2<< "\n";
		s << "Var1: "<< ent.mm6ent.var1<< "\t";
		s << "Var2: "<< ent.mm6ent.var2<< "\n";
		if( version > 6)
		{
			s << "Spec Trig: "<< std::hex << std::setw(4)<<std::setfill('0')  << ent.spec_trig<< "\t";
			s << "padding: "<< std::hex << std::setw(4)<<std::setfill('0')  << ent.padding<< "\n";
		}

		OverlayManager::getSingleton().getOverlayElement("MMView/ODMMap/EntityParams")->
			setCaption(s.str());

	}
	void    blvMap::DeselectEntity()
	{
		Overlay*mMyOverlay = OverlayManager::getSingleton().getByName("MMView/ODMMapEntityInfoOverlay");
		mMyOverlay->hide();
		if( selected_ent_idx == -1 )
			return;
		Entity* ent = mSceneMgr->getEntity("Entity" + StringConverter::toString(selected_ent_idx));
		if( !ent )
		{
			angel::Log << "could not find entity " << selected_ent_idx <<angel::aeLog::endl;
		}else
		{
			ent->getParentSceneNode()->showBoundingBox(false);
		}

		selected_ent_idx=-1;
	}
	void    blvMap::SelectSpawn()
	{
		if( selected_spawn_idx == -1 )
			return;
		Entity* ent_o = mSceneMgr->getEntity("Spawn" + StringConverter::toString(selected_spawn_idx));
		if( !ent_o )
		{
			angel::Log << "could not find spawn " << selected_spawn_idx <<angel::aeLog::endl;
		}else
		{
			ent_o->getParentSceneNode()->showBoundingBox(true);
		}
		Overlay*mMyOverlay = OverlayManager::getSingleton().getByName("MMView/ODMMapSpawnInfoOverlay");
		mMyOverlay->show();
		std::stringstream s;
		spawn_t& swp=spawns[selected_spawn_idx];
		s << "Spawn#: "<< selected_spawn_idx << "\t";
		s << "index: "<< swp.mm6spawn.index << "\n";

		s << "Origin: "<< swp.mm6spawn.origin<< "\t";
		s << "Radius: "<< swp.mm6spawn.radius<< "\n";

		s << "Type_ID: "<< std::hex << std::setw(4)<<std::setfill('0')  << swp.mm6spawn.type_id<< "\t";
		s << "Attrib: "<< std::hex << std::setw(4)<<std::setfill('0')  << swp.mm6spawn.attrib<< "\n";
		if( version > 6)
		{
			s << "Group: "<< std::hex << std::setw(8)<<std::setfill('0')  << swp.group<< "\n";

		}
		OverlayManager::getSingleton().getOverlayElement("MMView/ODMMap/SpawnParams")->
			setCaption(s.str());
	}
	void    blvMap::DeselectSpawn()
	{
		Overlay*mMyOverlay = OverlayManager::getSingleton().getByName("MMView/ODMMapSpawnInfoOverlay");
		mMyOverlay->hide();
		if( selected_spawn_idx == -1 )
			return;
		Entity* ent = mSceneMgr->getEntity("Spawn" + StringConverter::toString(selected_spawn_idx));
		if( !ent )
		{
			angel::Log << "could not find spawn " << selected_spawn_idx <<angel::aeLog::endl;
		}else
		{
			ent->getParentSceneNode()->showBoundingBox(false);
		}

		selected_spawn_idx=-1;

	}*/

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
