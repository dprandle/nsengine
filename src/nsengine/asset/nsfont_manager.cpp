/*! 
	\file nsfont_manager.cpp
	
	\brief Definition file for nsfont_manager class

	This file contains all of the neccessary definitions for the nsfont_manager class.

	\author Daniel Randle
	\date Feb 20 2016
	\copywrite Earth Banana Games 2013
*/

#include <nsplugin_manager.h>
#include <nsfont_manager.h>
#include <nstex_manager.h>
#include <nsutility.h>
#include <nsplatform.h>
#include <ft2build.h>
#include FT_FREETYPE_H

FT_LibraryRec_ * ftlib = nullptr;

nsfont * get_font(const uivec2 & id)
{
	nsplugin * plg = nsep.get(id.x);
	if (plg == nullptr)
		return nullptr;
	return plg->get<nsfont>(id.y);
}

nsfont_manager::nsfont_manager():
	nsasset_manager(type_to_hash(nsfont_manager))
{
	static bool ft_init = false;
	if (!ft_init)
	{
		uint8 error = FT_Init_FreeType( &ftlib);
		if ( error )
		{
			dprint("nsfont_manager::ctor Unable to initialize freetype library");
			return;
		}
		ft_init = true;
	}	
	set_local_dir(LOCAL_FONT_DIR_DEFAULT);
	set_save_mode(nsasset_manager::text);
}

nsfont_manager::~nsfont_manager()
{}

nsfont * nsfont_manager::create(uint32 res_type_id, const nsstring & res_name, nsasset * to_copy)
{
	nsfont * fnt = static_cast<nsfont*>(nsasset_manager::create(res_type_id, res_name, to_copy));
	if (fnt != nullptr && to_copy != nullptr)
		_load_font_faces(fnt);
	return fnt;
}

nsfont * nsfont_manager::load(uint32 res_type_id, const nsstring & fname, bool finalize_)
{
	if (fname.empty())
		return nullptr;
	
	nsstring font_extension = "";
	size_t ext_pos = fname.find_last_of(".");

	if (ext_pos != nsstring::npos)
		font_extension = fname.substr(ext_pos);
	else
	{
		dprint("nsfont_manager::load Cannot load font file with no extension");
		return NULL;
	}

	if (font_extension != DEFAULT_FONT_EXTENSION)
	{
		nsfont * fnt = import(fname);
        if (fnt != nullptr && finalize_)
            fnt->finalize();
		return fnt;
	}
	else
	{
		nsfont * fnt = static_cast<nsfont*>(nsasset_manager::load(res_type_id, fname, finalize_));
		_load_font_faces(fnt);
		return fnt;
	}
}

nsfont * nsfont_manager::import(const nsstring & fname)
{
	nsfont * new_fnt = create(name_from_filename(fname));
	if (new_fnt == nullptr)
		return nullptr;

	platform::read(fname, &new_fnt->m_font_file_data);

	if (!_load_font_faces(new_fnt))
		return nullptr;

	return new_fnt;
}

bool nsfont_manager::_load_font_faces(nsfont * fnt)
{
	fnt->m_faces.push_back(font_face());	
	uint8 error = FT_New_Memory_Face(
		ftlib,
		fnt->m_font_file_data.data(),
		fnt->m_font_file_data.size(),
		0,
		&fnt->m_faces.back().ft_face);

	if ( error == FT_Err_Unknown_File_Format )
	{
		dprint("nsfont_manager::import - Uknown font file format");
		destroy(fnt);
		return false;
	}
	else if ( error )
	{
		dprint("nsfont_manager::import - Uknown font file error");
		destroy(fnt);
		return false;
	}

	uint8 num_faces = fnt->m_faces.back().ft_face->num_faces;
	if (num_faces > 255)
	{
		dprint("nsfont_manager::import - Cannot load font with more than 255 faces");
		destroy(fnt);
		return false;
	}

	nsstring tex_name(fnt->m_faces.back().ft_face->family_name);
	tex_name += " " + nsstring(fnt->m_faces.back().ft_face->style_name);
	fnt->m_faces.back().atlas = nsep.get(m_plugin_id)->create<nstex2d>(tex_name);
	fnt->m_faces.back().atlas->set_format(tex_red);
	fnt->m_faces.back().atlas->set_component_data_type(tex_u8);
	fnt->m_faces.back().atlas->enable_group_save(false);
	for (uint8 i = 1; i < num_faces; ++i)
	{
		fnt->m_faces.push_back(font_face());	
		uint8 error = FT_New_Memory_Face(
			ftlib,
			fnt->m_font_file_data.data(),
			fnt->m_font_file_data.size(),
			i,
			&fnt->m_faces.back().ft_face);

		if ( error == FT_Err_Unknown_File_Format )
		{
			dprint("nsfont_manager::import - Uknown font file format");
			destroy(fnt);
			return false;
		}
		else if ( error )
		{
			dprint("nsfont_manager::import - Uknown font file error");
			destroy(fnt);
			return false;
		}

		nsstring tex_name(fnt->m_faces.back().ft_face->family_name);
		tex_name += " " + nsstring(fnt->m_faces.back().ft_face->style_name);
		fnt->m_faces.back().atlas = nsep.get(m_plugin_id)->create<nstex2d>(tex_name);
		fnt->m_faces.back().atlas->set_format(tex_red);
		fnt->m_faces.back().atlas->set_component_data_type(tex_u8);
		fnt->m_faces.back().atlas->enable_group_save(false);
	}
	fnt->_update_faces();
	return true;
}
