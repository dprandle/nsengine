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

nsfont * get_font(const uivec2 & id)
{
	nsplugin * plg = nsep.get(id.x);
	if (plg == nullptr)
		return nullptr;
	return plg->get<nsfont>(id.y);
}

nsfont_manager::nsfont_manager():
	nsres_manager(type_to_hash(nsfont_manager))
{
	set_local_dir(LOCAL_FONT_DIR_DEFAULT);
	set_save_mode(nsres_manager::text);
}

nsfont_manager::~nsfont_manager()
{}

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

	if (font_extension == ".fnt" || font_extension == ".FNT")
	{
		nsfont * fnt = load_external(fname);
		if (fnt != nullptr && finalize_)
			fnt->finalize();
		return fnt;
	}
	else
		return static_cast<nsfont*>(nsres_manager::load(res_type_id, fname, finalize_));
}

nsfont * nsfont_manager::load_external(const nsstring & fname)
{
	nsstring res_name(name_from_filename(fname));
	nsfstream file;
	file.open(fname, nsfstream::in);
	if (!file.is_open())
	{
		dprint("nsfont_manager::load_external Error opening resource file " + fname);
		return nullptr;
	}

	nsfont * res = get(res_name);
	if (res == nullptr)
	{
		res = create(res_name);
	}
	else
	{
		dprint("nsfont_manager::load_external - Error trying to load external font with same name as already existing font - " + res_name);
		file.close();
		return nullptr;
	}	

	res->set_ext(DEFAULT_FONT_EXTENSION);
	nsstring line;
	uint8 page_index = 0;
	uint8 chars_check = 0;
	uint8 chars_index = 0;

	while (!file.eof())
	{
		std::getline(file, line);
		if (line.find("info") == 0)
		{
			parse_info_line(line, res->get_font_info());
		}
		else if (line.find("common") == 0)
		{
			parse_common_line(line, res->get_font_info());
		}
		else if (line.find("page") == 0)
		{
			nsstring tex_name;
			nstex2d * t = nullptr;
			auto tm = nsep.get(m_plugin_id)->manager<nstex_manager>();
			nsstring path = path_from_filename(fname);
			uint8 pid = tex_name_from_line(line, tex_name);

			if (pid != page_index)
			{
				dprint("nsfont_manager::load_external - Warning - current page index does not match file page index");
			}
			
			t = tm->load_image(path+tex_name);
			if (t == nullptr)
				t = tm->load<nstex2d>(tex_name, true);
			if (chars_check != chars_index)
			{
				dprint("nsfont_manager::load_external - Warning - character index for page " + std::to_string(page_index) + " does not match character count");
			}
			
			res->push_page(t->full_id());
			++page_index;
		}
		else if (line.find("chars") == 0)
		{
			chars_check = parse_count_from_line(line);
		}
		else if (line.find("char") == 0)
		{
			parse_char_line(line, res);
			++chars_index;
		}
		else if (line.find("kernings") == 0)
		{
			dprint("nsfont_manager::load_external Still need to implement kernings");
		}
		else
		{
			dprint("nsfont_manager::load_external Unrecognized input in font file");
		}
	}
	if (res->get_font_info().page_count != page_index)
	{
		dprint("nsfont_manager::load_external - Warning - page count does not match page index for font " + res_name);
	}
	return res;	
}

void parse_char_line(const nsstring & l, nsfont * fnt)
{
	nsstring_vector split_strs;
	nsstring_vector tmp_split;
	split_strs.reserve(12);
	tmp_split.reserve(2);
	
	util::split(l, ' ', split_strs);

	uint8 id;
	util::split(split_strs[1], '=', tmp_split);
	id = std::stoi(tmp_split[1]);
	
	char_info & ci = fnt->get_char_info(id);
	
	for (uint8 i = 2; i < 11; ++i)
	{
		tmp_split.resize(0);
		util::split(split_strs[i],'=',tmp_split);
		uint32 val = std::stoi(tmp_split[1]);
		if (i >= 2 && i < 6)
			ci.rect[i-2] = val;
		else if (i >= 6 && i < 8)
			ci.offset[i-6] = val;
		else if (i == 8)
			ci.xadvance = (uint8)val;
		else if (i == 9)
			ci.page_index = (uint8)val;
		else
			ci.channel = (uint8)val;
	}
}

void parse_common_line(const nsstring & l, font_info & fi)
{
	nsstring_vector split_strs;
	nsstring_vector tmp_split;
	nsstring_vector padd;
	padd.reserve(2);
	split_strs.reserve(12);
	tmp_split.reserve(2);
	util::split(l, ' ', split_strs);	
	for (uint8 i = 1; i < 7; ++i)
	{
		tmp_split.resize(0);
		util::split(split_strs[i],'=',tmp_split);
		switch(i)
		{
		  case(1):
			  fi.line_height = stoi(tmp_split[1]);
			  break;
		  case(2):
			  fi.base = stoi(tmp_split[1]);
			  break;
		  case(5):
			  fi.page_count = stoi(tmp_split[1]);
			  break;
		  case(6):
			  fi.packed = stoi(tmp_split[1]);
			  break;
		}
	}
}

void parse_info_line(const nsstring & l, font_info & fi)
{
	nsstring_vector split_strs;
	nsstring_vector tmp_split;
	nsstring_vector padd;
	padd.reserve(4);
	split_strs.reserve(12);
	tmp_split.reserve(2);
	util::split(l, ' ', split_strs);	
	for (uint8 i = 1; i < 12; ++i)
	{
		tmp_split.resize(0);
		util::split(split_strs[i],'=',tmp_split);
		switch(i)
		{
		  case(1):
			  fi.face = tmp_split[1].substr(1, tmp_split[1].size()-2);
			  break;
		  case(2):
			  fi.pt = stoi(tmp_split[1]);
			  break;
		  case(3):
			  fi.bold = stoi(tmp_split[1]);
			  break;
		  case(4):
			  fi.italic = stoi(tmp_split[1]);
			  break;
		  case(5):
			  fi.charset = tmp_split[1].substr(1, tmp_split[1].size()-2);
			  break;
		  case(6):
			  fi.unicode = stoi(tmp_split[1]);
			  break;
		  case(7):
			  fi.stretch = stoi(tmp_split[1]);
			  break;
		  case(8):
			  fi.smooth = stoi(tmp_split[1]);
			  break;
		  case(9):
			  fi.super_sampling = stoi(tmp_split[1]);
			  break;
		  case(10):
			  padd.resize(0);
			  util::split(tmp_split[1],',', padd);
			  for (uint8 i = 0; i < 4; ++i)
				  fi.padding[i] = std::stoi(padd[i]);
			  break;
		  case(11):
			  padd.resize(0);
			  util::split(tmp_split[1],',', padd);
			  for (uint8 i = 0; i < 2; ++i)
				  fi.spacing[i] = std::stoi(padd[i]);
			  break;
		}
	}
}

uint8 tex_name_from_line(const nsstring & l, nsstring & name)
{
	nsstring_vector split_strs;
	nsstring_vector tmp_split;
	uint8 ret = 0;
	split_strs.reserve(3);
	tmp_split.reserve(2);
	util::split(l, ' ', split_strs);
	for (uint8 i = 1; i < 3; ++i)
	{
		tmp_split.resize(0);
		util::split(split_strs[i], '=', tmp_split);
		name = tmp_split[1];
		if (i == 1)
			ret = stoi(name);
	}
	name = name.substr(1, name.size()-2);
	return ret;
}

uint8 parse_count_from_line(const nsstring & l)
{
	nsstring_vector splt = util::split(util::split(l, ' ')[1], '=');
	return stoi(splt[1]);
}
