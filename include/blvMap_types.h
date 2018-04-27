
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
#ifndef _ae_blvMap_types
#define _ae_blvMap_types
#include "aeGeometry.h"
#include "aeTexture.h"
#include <learnopengl/mesh.h>
namespace angel{
        /*
        typedef struct
        {
            int v_idx;
            float   hz[3];
            float	tex_c[2];
            vec3_t	vertex;
        }node_t;

        typedef struct
        {
            std::string texname;
            p_aeTexture tex;

            plane_t plane,plane7;
            bbox_t	bbox;
            int		areas[2];
            int		fparm_index;
            //unknown data
            uint8_t	data0x10[0x10],b0x4c;
            uint16_t	b0x4e;

            std::vector<node_t> nodes;
            uint8_t    *bindata;
            int		index,offv;
        }face_t;*/
#pragma pack(push,1)
		struct blv_face_t
		{//starts at 0x10 in mm7,mm8
			mm_int_plane_s plane;					//0x00
			int16_t z_calc[6];						//0x10 Z_CALC1,Z_CALC2,Z_CALC3,
			uint32_t attrib;								//0x1c
			uint32_t VERTEX_INDEX_OFFSET,				//0x20 //not used(fills on load)
				X_DISPLACEMENT_OFFSET,
				Y_DISPLACEMENT_OFFSET,
				Z_DISPLACEMENT_OFFSET,
				U_TEXTURE_OFFSET,					//0x30
				V_TEXTURE_OFFSET;
			uint16_t        fparm_index;				//0x38
			uint16_t		bitmap_index;			//0x3a
			uint16_t	    room_number,				//0x3c
						   room_behind_number;		//0x3e
			mm_short_bbox_s bbox;					//0x40
			uint8_t facet_type,numvertex;				//0x4c
			uint8_t padding[2];						//0x4e (unused?)
		};
		struct blv_faceparams_t
		{
			mm_int_vec3_s	fade;					//0x00
			uint16_t	facet_index;					//0x0c
			uint16_t	bitmap_index;					//0x0e
			uint16_t	texture_frame_table_index;		//0x10
			uint16_t	texture_frame_table_COG;		//0x12
			int16_t tex_u;							//0x14 tex_u
			int16_t tex_v;							//0x16 tex_u
			uint16_t  cog_number;						//0x18 
			uint16_t  cog_triggered;					//0x1A event index
			uint16_t  cog_trigger_type;					//0x1c
			int16_t	fade_base_x;					//0x1e
			int16_t	fade_base_y;					//0x20
			uint16_t	light_level;					//0x22
		};
		struct blv_faceparams2_t
		{
            uint8_t unk[0xa];
        };
		struct blv_spawn_t
		{
			mm_int_vec3_s origin;			//0x00
			uint16_t radius;					//0x0c
			uint16_t type_id;					//0x0e
			uint16_t index;						//0x10
			uint16_t attrib;					//0x12
		};
		struct blv_ent_t
		{
			uint16_t declist_id;				//0x00
			uint16_t AIattrMarkers;				//0x02
			mm_int_vec3_s origin;			//0x04
			int facing;						//0x10
			uint16_t evt1,evt2;					//0x14
			uint16_t var1,var2;					//0x18
		};

#pragma pack(pop)
        struct face_t
        {
            std::string texname;
            std::shared_ptr<aeTexture> tex;
            std::shared_ptr<Mesh> mesh;
            
            mm_float_plane_s blv_plane7;
            blv_face_t blv_face; 
            blv_faceparams_t blv_faceparam;
            blv_faceparams2_t blv_faceparam2;
			std::vector<uint16_t> vertex_idx;
			std::vector<int16_t> vertex_normal_x;
			std::vector<int16_t> vertex_normal_y;
			std::vector<int16_t> vertex_normal_z;

			std::vector<int16_t> vertex_tex_x;
			std::vector<int16_t> vertex_tex_y;
        };
}
#endif //_ae_blvMap_types
