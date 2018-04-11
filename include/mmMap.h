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
#ifndef _ae_mmMap
#define _ae_mmMap
#include <vector>
#include <string>
#include "lodfile.h"

namespace angel{
	class mmMap{
    private:
		std::string mapname;
    protected:
        int version;
        pLodData loddata;
	public:
		mmMap( pLodData _loddata, const std::string name ):
         mapname(name), loddata(_loddata)
        {
        }
		~mmMap(){};
		virtual void Draw()=0;
		int MapVersion() { return version;}  
		virtual void TogglePortals()=0;
		virtual void ToggleEnts()=0;
		const std::string& MapName(){ return mapname;}
		virtual void Select()=0;
    }

	class blvMap:public mmMap
    {
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
        }face_t;

        private:
            //uint32_t    datasize;

            short   *vertex_data;
            int		num_vertex;
            uint8_t    *faces_array;
            int		num_faces;
            short   *facedata;
            int     facedatasize;
            uint8_t    *facetextures;

            int		numfaceparms;
            uint8_t    *faceparams1;
            uint8_t    *faceparams2;

            int		num_sectors;
            uint8_t	*sectorsdata;
            int		Rdatasize;
            uint8_t	*sectorsRdata;
            int		RLdatasize;
            uint8_t	*sectorsRLdata;
            int		num_sprites_hz;
            int		num_sprites;
            uint8_t	*spritesdata;
            uint8_t	*spritesnamesdata;
            int		num_lights;
            uint8_t	*lightsdata;
            int		num_unk9;
            uint8_t	*Unknown9data;
            int		num_spawn;
            uint8_t	*spawndata;
            int		num_outline;
            uint8_t	*mapoutlinedata;

            int		facesize;
            int		spritesize;
            int		lightsize;
            int		spawnsize;
            int		sectorsize;
            
            bool	showportals;

            std::vector<vec3_t> vertexes;
            std::vector<face_t> faces;
//            face_t* selected_face;

            bool    PrepareBLV();
            bool    DetectBLVVersion();
            void	LoadBLVMap();
            //bool	isPortal( const   face_t & face );
            //void	RenderPortals();
        public:
            blvMap( pLodData _loddata, const std::string name );
            ~blvMap();
            virtual void Draw();
            virtual void TogglePortals();
            virtual void ToggleEnts();
            virtual void Select();
    };
}
#endif //_ae_blvMap
