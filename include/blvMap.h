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
#ifndef _ae_blvMap
#define _ae_blvMap
#include "mmMap.h"
#include "blvMap_types.h"

namespace angel{
	class blvMap:public mmMap
    {
		struct blv_map_data_t 
        {
            short   *vertex_data;
            int		num_vertex;
            uint8_t    *faces_array;
            int		num_faces;
            int16_t   *facedata;
            int     facedatasize;
            uint8_t    *facetextures;

            int		numfaceparms;
            blv_faceparams_t  *faceparams1;
            blv_faceparams2_t  *faceparams2;

            int		num_sectors;
            uint8_t	*sectorsdata;
            int		Rdatasize;
            uint8_t	*sectorsRdata;
            int		RLdatasize;
            uint8_t	*sectorsRLdata;
            int		num_sprites_hz;
            int		num_sprites;
            uint8_t	*spritesdata;
            char	*spritesnamesdata;
            int		num_lights;
            uint8_t	*lightsdata;
            int		num_unk9;
            uint8_t	*Unknown9data;
            int		num_spawn;
            uint8_t	*spawndata;
            int		num_outline;
            uint8_t	*mapoutlinedata;
        };
        struct blv_map_sizes_t
        {
            int		facesize;
            int		spritesize;
            int		lightsize;
            int		spawnsize;
            int		sectorsize;
        };

        private:
            uint32_t    datasize;
            uint8_t *data;
            blv_map_data_t map_data;
            blv_map_sizes_t map_sizes;
            
            bool	showportals;
            std::vector<mm_short_vec3_s> vertexes;

            std::vector<face_t> faces;
            std::vector<std::string> texnames;

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
