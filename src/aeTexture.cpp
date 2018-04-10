/*

Copyright (C) 2009 Angel (angel.d.death@gmail.com)

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
#include "aeTexture.h"
//#include <libgfl.h>
#include <zlib.h>
#include <algorithm>
#include <stb_image.h>
#include "log.h"
#include "lodfile.h"

aeTextureManager TexManager;
using namespace angel;

static const std::string typenames[] = {
		"data/textures/common/",
		"data/textures/sprite/",
		"data/textures/interface/",
		"data/fonts/"
};
bool aeTexture::LoadImage( )
{
	std::string fname( basename );
	angel::Log << angel::aeLog::debug <<"aeTexture::LoadImage: Loading texture "<< fname << angel::aeLog::endl;
    
    int width, height, nrChannels;
    unsigned char *data = stbi_load(fname.c_str(), &width, &height, &nrChannels, 0);
    if( !data ){
        angel::Log << angel::aeLog::debug <<"aeTexture::LoadPNG: Failed loading texture "<< fname << angel::aeLog::endl;
		return false;
    }
    GLuint format;
    switch( nrChannels )
    {
        case 4:
            format = GL_RGBA;
            break;
        case 3:
            format = GL_RGB;
            break;
        default:
            angel::Log << "LoadPNG: " << fname << " : Invalid PNG bpp " << bpp << angel::aeLog::endl;
            return false;
    }
	glGenTextures( 1, ( GLuint * ) & GLID);
	glBindTexture( GL_TEXTURE_2D,  GLID );
	bool makemipmap = false;
	if( type == TT_Texture )
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		makemipmap  = true;
	}else
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
		if( type == TT_Sprite)
			makemipmap =true;
	}
    
    glTexImage2D( GL_TEXTURE_2D, 0, format, width , height, 0,	format, GL_UNSIGNED_BYTE, data );
	if( makemipmap )
	{
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); //GL_LINEAR
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glGenerateMipmap(GL_TEXTURE_2D);
	}else
	{
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); //GL_LINEAR
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	}
    stbi_image_free(data);

	glBindTexture( GL_TEXTURE_2D,  0 );
	//CheckGlError("aeTexture::LoadPNG");
	return true;
}

bool CheckPOW2( int n )
{
	int n1 = 1;
	while( n1 < n ) n1<<=1;

	return ( n1 == n );
}
static const std::string inloddir[] = {
	    "bitmaps/",
		"sprites08/",
		"icons/",
		"fonts/"
};
bool aeTexture::LoadSpriteFromLod( )
{
	/*std::string fname = inloddir[type]+basename;
	int size;
	uint8_t*data = LodManager.LoadFile( fname , &size);
	if(!data)
		return false;
	boost::scoped_array<uint8_t> sdata(data); 
	int psize = *(int*)(data+0x0c);
	unsigned long unpsize = *(int*)(data+0x1c);
	width  = *(uint16_t*)(data+0x10);
	height = *(uint16_t*)(data+0x12);

	if( psize+0x20+height*8 != size )
		return false;
	uint8_t*unpdata = new uint8_t[unpsize];
	boost::scoped_array<uint8_t> sunpdata(unpdata);
	if ( uncompress( unpdata, &unpsize , data + 0x20+height*8, psize ) != Z_OK )
		return false;
	int palindex = *(int*)(data+0x14);
	boost::format palname("bitmaps/pal%03d");
	palname	% palindex;
	int sizepal;
	uint8_t *pal = LodManager.LoadFile( palname.str() , &sizepal);
	boost::scoped_array<uint8_t> spal(pal);
	if( sizepal != 0x330 )
		return false;
	
	

	bpp = 4;
	uint8_t * imgdata = new uint8_t[width*height*4];
	boost::scoped_array<uint8_t> simgdata(imgdata);
	memset(imgdata,0,width*height*4);
	for ( int i = 0; i< height;i++)
	{
		int off = *(int*)(data+0x20+i*8+4);
		int x1 = *(uint16_t*)(data+0x20+i*8+0);
		int x2 = *(uint16_t*)(data+0x20+i*8+2);
		int imgoff = ( height - i - 1)*width*4+(x1)*4;
		if( x1 < 0 || x2 < 0 || x1 > width || x2 > width || x2 < x1 )
			continue;
		for ( int j = 0 ; j < x2-x1+1 ; j++ )
		{
			int cindex = unpdata[off+j];
			imgdata[imgoff+j*4 + 0 ] = pal[0x30+cindex*3+0];
			imgdata[imgoff+j*4 + 1 ] = pal[0x30+cindex*3+1];
			imgdata[imgoff+j*4 + 2 ] = pal[0x30+cindex*3+2];
			if( cindex )
				imgdata[imgoff+j*4 + 3 ] = 0xff;
			else
				imgdata[imgoff+j*4 + 3 ] = 0;
		}
	}
	glGenTextures( 1, ( GLuint * ) & GLID);
	glBindTexture( GL_TEXTURE_2D,  GLID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); //GL_LINEAR
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	int w = 1;while( w < width)w<<=1;
	int h = 1;while( h < height)h<<=1;
	if( w!=width || h != height )
	{
		uint8_t* newimgdata = new uint8_t[w*h*4];
		if(gluScaleImage(GL_RGBA,width,height, GL_UNSIGNED_BYTE, imgdata,
			w,h,GL_UNSIGNED_BYTE,newimgdata)!=0)
		{
			delete[] newimgdata;
			return false;
		}
		width = w;
		height = h;
		gluBuild2DMipmaps( GL_TEXTURE_2D, bpp, width , height, GL_RGBA, GL_UNSIGNED_BYTE, newimgdata );
		delete[] newimgdata;
	}else
	{
		gluBuild2DMipmaps( GL_TEXTURE_2D, bpp, width , height, GL_RGBA, GL_UNSIGNED_BYTE, imgdata   );
	}
	boost::format fmt(" w:%d h:%d ");
	fmt % width % height;
	angel::Log << angel::aeLog::debug << "aeTexture::LoadSpriteFromLod: Loading sprite "<< fname << fmt << angel::aeLog::endl;
	//CheckGlError("aeTexture::LoadSpriteFromLod");
	return true;*/
    return false;
}
bool aeTexture::LoadFromLod( )
{
	if( type == TT_Sprite )
		return LoadSpriteFromLod();
	std::string fname = inloddir[type]+basename;
	auto ldata = LodManager.LoadFile( fname );
	if(!ldata){
        angel::Log << angel::aeLog::debug <<"aeTexture::LoadFromLod: Failed loading texture "<< fname << angel::aeLog::endl;
		return false;
    }
    uint8_t*data= &((*ldata)[0]);
    uint32_t size = (uint32_t)ldata->size();
	uint32_t psize = *(uint32_t*)(data+0x14);
	uint32_t unpsize1 = *(uint32_t*)(data+0x10);
	uLongf unpsize2 = *(uint32_t*)(data+0x28);
	if( psize+0x30+0x300 != size )
		return false;
	if( unpsize2 && unpsize2 < unpsize1)
		return false;
	uint8_t* pal = data + 0x30 + psize;
	uint8_t*unpdata = new uint8_t[unpsize2 ];

	if ( uncompress( unpdata, &unpsize2 , data + 0x30, psize ) != Z_OK )
    {
        delete[] unpdata;
		return false;
    }
	
	
	width  = *(int16_t*)(data+0x18);
	height = *(int16_t*)(data+0x1a);
	bpp = 4;
	if( !CheckPOW2(width) || !CheckPOW2(height))
		return false;

	int imgsize = width*height;
	uint8_t* rgbdata = new uint8_t[imgsize*bpp];


	glGenTextures( 1, ( GLuint * ) & GLID);
	glBindTexture( GL_TEXTURE_2D,  GLID );
	if( type == TT_Texture )
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	}else
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	}
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); //GL_LINEAR
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    int n=0,off=0; 
    
    for ( int i = 0; i < imgsize; i++)
    {
        int index = unpdata[off+i];
        int r = pal[index*3+0];
        int g = pal[index*3+1];
        int b = pal[index*3+2];

        if( index == 0 && ((r == 0 && g >250 && b > 250) || (r > 250 && g ==0 && b > 250)))
        {
            r=g=b=0;
            rgbdata[i*bpp+3]= 0;
        }
        else
            rgbdata[i*bpp+3]= 0xff;

        rgbdata[i*bpp+0]= r;
        rgbdata[i*bpp+1]= g;
        rgbdata[i*bpp+2]= b;
    }
    glTexImage2D( GL_TEXTURE_2D, 0, bpp, width , height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbdata );
    glGenerateMipmap(GL_TEXTURE_2D);
    delete[] rgbdata;
    delete[] unpdata;
	
    //boost::format fmt(" w:%d h:%d %d mipmaps");
	//fmt % width % height % n;
	angel::Log << angel::aeLog::debug << "aeTexture::LoadFromLod: Loading texture "<< fname << angel::aeLog::endl;
	glBindTexture( GL_TEXTURE_2D,  0 );
	//CheckGlError("aeTexture::LoadFromLod");
	return true;
}
aeTexture::aeTexture( const std::string& tname, const std::string& basename, uTexType  textype):
name( tname ), basename(basename), type(textype)
{
	if( type != TT_Font && LoadFromLod() )
		return;
	
	if( LoadImage() )
		return;
	
	//default texture
	const static uint8_t data[] = {  
		0,0xff,0xff,0xff,
		0,0xff,0xff,0xff,
		0,0xff,0xff,0xff,
		0,0xff,0xff,0xff
	};
	width = 2;
	height = 2;
	bpp = 4;
	angel::Log << "Cannot Load texture " << tname << " generate default"<< angel::aeLog::endl;
	glGenTextures( 1, ( GLuint * ) & GLID);
	glBindTexture( GL_TEXTURE_2D,  GLID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); //GL_LINEAR
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, 4, 2, 2, 0, GL_RGBA	, GL_UNSIGNED_BYTE, data );
	glTexImage2D( GL_TEXTURE_2D, 1, 4, 1, 1, 0, GL_RGBA	, GL_UNSIGNED_BYTE, data );
	
//	throw error( (boost::format("aeTexture:Cannot Load Texture %s") % fname).str() );
}

aeTexture::~aeTexture()
{
	Disable();
	glDeleteTextures(1, &GLID);
	angel::Log << angel::aeLog::debug << "Remove texture "<< name << angel::aeLog::endl;
}


aeTextureManager::aeTextureManager()
{
}

aeTextureManager::~aeTextureManager()
{
/*	texmap_iter_t ii = items.begin();
	for( ; ii != items.end(); ++ii )
	{
	        p_aeTexture p = (*ii).second.lock();
            if( p )
            {
                        p.reset();      	//текстура (*ii).first не освобождена
        	}
    }*/
}

std::shared_ptr<aeTexture>     aeTextureManager::GetTexture(const std::string& name, uTexType  textype)
{
	std::string tname( typenames[textype] + name );

	std::transform( tname.begin(), tname.end(), tname.begin(), tolower);

	texmap_iter_t ii = items.find( tname );

	if( ii != items.end() )
	{
		auto p = (*ii).second.lock();
		if( p )
			return p;
		p = std::shared_ptr<aeTexture>( new aeTexture(tname, name, textype));
		(*ii).second = p;
		return p;
	}
    std::shared_ptr<aeTexture> p( new aeTexture(tname, name, textype));
	items[tname] = p;

	return std::shared_ptr<aeTexture>(p);
}

void            aeTextureManager::RemoveTexture( const std::string& name )
{
	texmap_iter_t ii = items.find( name );
	if( ii == items.end() )
	{
		angel::Log << angel::aeLog::debug << "RemoveTexture: текстура" << name << " не найдена" << angel::aeLog::endl;
		return;
	}
	items.erase(ii);
}
