#include <nspupper.h>

nsfile_pupper::nsfile_pupper(pup_t t_, nsfstream & fileStream, const uint32 & _io) : m_type(t_), fs(fileStream), m_io(_io) {}

nsfile_pupper::pup_t nsfile_pupper::type()
{
	return m_type;
}

const uint32 & nsfile_pupper::mode() const
{
	return m_io;
}

nsbinary_file_pupper::nsbinary_file_pupper(nsfstream & fileStream, const uint32 & _io) :nsfile_pupper(pup_binary, fileStream,_io){}

void pup(nsbinary_file_pupper & p, char & val, const nsstring & varName)
{
	pup_bytes(p, val);
}

void pup(nsbinary_file_pupper & p, wchar & val, const nsstring & varName)
{
	pup_bytes(p, val);
}

#ifndef WIN32
void pup(nsbinary_file_pupper & p, char16 & val, const nsstring & varName)
{
	pup_bytes(p, val);
}

void pup(nsbinary_file_pupper & p, char32 & val, const nsstring & varName)
{
	pup_bytes(p, val);
}
#endif

void pup(nsbinary_file_pupper & p, int8 & val, const nsstring & varName)
{
	pup_bytes(p, val);
}

void pup(nsbinary_file_pupper & p, int16 & val, const nsstring & varName)
{
	pup_bytes(p, val);
}

void pup(nsbinary_file_pupper & p, int32 & val, const nsstring & varName)
{
	pup_bytes(p, val);
}

void pup(nsbinary_file_pupper & p, int64 & val, const nsstring & varName)
{
	pup_bytes(p, val);
}

void pup(nsbinary_file_pupper & p, uint8 & val, const nsstring & varName)
{
	pup_bytes(p, val);
}

void pup(nsbinary_file_pupper & p, uint16 & val, const nsstring & varName)
{
	pup_bytes(p, val);
}

void pup(nsbinary_file_pupper & p, uint32 & val, const nsstring & varName)
{
	pup_bytes(p, val);
}

void pup(nsbinary_file_pupper & p, uint64 & val, const nsstring & varName)
{
	pup_bytes(p, val);
}

void pup(nsbinary_file_pupper & p, float & val, const nsstring & varName)
{
	pup_bytes(p, val);
}

void pup(nsbinary_file_pupper & p, double & val, const nsstring & varName)
{
	pup_bytes(p, val);
}

void pup(nsbinary_file_pupper & p, ldouble & val, const nsstring & varName)
{
	pup_bytes(p, val);
}

void pup(nsbinary_file_pupper & p, bool & val, const nsstring & varName)
{
	pup_bytes(p, val);
}


nstext_file_pupper::nstext_file_pupper(nsfstream & fileStream, const uint32 & _io) :nsfile_pupper(pup_text, fileStream, _io){}

void pup(nstext_file_pupper & p, char & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<char>"; endtag = "</char>";

	if (p.m_io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		size_t beg = begtag.size(); size_t loc = line.find(endtag);
		line = line.substr(beg, loc-beg);
		val = line[0];
	}
}

void pup(nstext_file_pupper & p, wchar & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<wchar>"; endtag = "</wchar>";

	if (p.m_io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		size_t beg = begtag.size(); size_t loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = line[0];
	}
}

#ifndef WIN32
void pup(nstext_file_pupper & p, char16 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<char16>"; endtag = "</char16>";

	if (p.m_io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		size_t beg = begtag.size(); size_t loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = line[0];
	}
}

void pup(nstext_file_pupper & p, char32 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<char32>"; endtag = "</char32>";

	if (p.m_io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		size_t beg = begtag.size(); size_t loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = line[0];
	}
}
#endif

void pup(nstext_file_pupper & p, int8 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<int8>"; endtag = "</int8>";

	if (p.m_io == PUP_OUT)
	{
		p.fs << begtag << (int32)val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		size_t beg = begtag.size(); size_t loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
        val = (int8)std::stoi(line);
	}
}

void pup(nstext_file_pupper & p, int16 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<int16>"; endtag = "</int16>";

	if (p.m_io == PUP_OUT)
	{
		p.fs << begtag << (int32)val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		size_t beg = begtag.size(); size_t loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = (int16)std::stoi(line);
	}
}

void pup(nstext_file_pupper & p, int32 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<int32>"; endtag = "</int32>";

	if (p.m_io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		size_t beg = begtag.size(); size_t loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = std::stoi(line);
	}
}

void pup(nstext_file_pupper & p, int64 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<int64>"; endtag = "</int64>";

	if (p.m_io == PUP_OUT)
	{
		p.fs << begtag << (int32)val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		size_t beg = begtag.size(); size_t loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = (int64)std::stol(line);
	}
}

void pup(nstext_file_pupper & p, uint8 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<uint8>"; endtag = "</uint8>";

	if (p.m_io == PUP_OUT)
	{
		p.fs << begtag << (uint32)val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		size_t beg = begtag.size(); size_t loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = (uint8)std::stoul(line);
	}
}

void pup(nstext_file_pupper & p, uint16 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<uint16>"; endtag = "</uint16>";

	if (p.m_io == PUP_OUT)
	{
		p.fs << begtag << (uint32)val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		size_t beg = begtag.size(); size_t loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = (uint16)std::stoul(line);
	}
}

void pup(nstext_file_pupper & p, uint32 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<uint32>"; endtag = "</uint32>";

	if (p.m_io == PUP_OUT)
	{
		p.fs << begtag << (uint32)val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		size_t beg = begtag.size(); size_t loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = (uint32)std::stoul(line);
	}
}

void pup(nstext_file_pupper & p, uint64 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<uint64>"; endtag = "</uint64>";

	if (p.m_io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		size_t beg = begtag.size(); size_t loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = std::stoull(line);
	}
}

void pup(nstext_file_pupper & p, float & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<float>"; endtag = "</float>";

	if (p.m_io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		size_t beg = begtag.size(); size_t loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
        if (line == "nan")
            val = 0;
        else
            val = std::stof(line);
	}
}

void pup(nstext_file_pupper & p, double & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<double>"; endtag = "</double>";

	if (p.m_io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		size_t beg = begtag.size(); size_t loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = std::stod(line);
	}
}

void pup(nstext_file_pupper & p, ldouble & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<ldouble>"; endtag = "</ldouble>";

	if (p.m_io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		size_t beg = begtag.size(); size_t loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = std::stold(line);
	}
}

void pup(nstext_file_pupper & p, bool & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<bool>"; endtag = "</bool>";

	if (p.m_io == PUP_OUT)
	{
		if (val)
			p.fs << begtag << "true" << endtag << "\n";
		else
			p.fs << begtag << "false" << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		size_t beg = begtag.size(); size_t loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = (line == "true");
	}
}

