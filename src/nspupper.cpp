#include <nspupper.h>

NSFilePUPer::NSFilePUPer(PupType t_, nsfstream & fileStream, const uint32 & _io) : t(t_), fs(fileStream), io(_io) {}

NSFilePUPer::PupType NSFilePUPer::type()
{
	return t;
}

const uint32 & NSFilePUPer::mode() const
{
	return io;
}

NSBinFilePUPer::NSBinFilePUPer(nsfstream & fileStream, const uint32 & _io) :NSFilePUPer(Binary, fileStream,_io){}

void pup(NSBinFilePUPer & p, char & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, wchar & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, char16 & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, char32 & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, int8 & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, int16 & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, int32 & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, int64 & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, uint8 & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, uint16 & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, uint32 & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, uint64 & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, float & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, double & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, ldouble & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, bool & val, const nsstring & varName)
{
	pupbytes(p, val);
}


NSTextFilePUPer::NSTextFilePUPer(nsfstream & fileStream, const uint32 & _io) :NSFilePUPer(Text, fileStream, _io){}

void pup(NSTextFilePUPer & p, char & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<char>"; endtag = "</char>";

	if (p.io == PUP_OUT)
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

void pup(NSTextFilePUPer & p, wchar & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<wchar>"; endtag = "</wchar>";

	if (p.io == PUP_OUT)
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

void pup(NSTextFilePUPer & p, char16 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<char16>"; endtag = "</char16>";

	if (p.io == PUP_OUT)
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

void pup(NSTextFilePUPer & p, char32 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<char32>"; endtag = "</char32>";

	if (p.io == PUP_OUT)
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

void pup(NSTextFilePUPer & p, int8 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<int8>"; endtag = "</int8>";

	if (p.io == PUP_OUT)
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

void pup(NSTextFilePUPer & p, int16 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<int16>"; endtag = "</int16>";

	if (p.io == PUP_OUT)
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

void pup(NSTextFilePUPer & p, int32 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<int32>"; endtag = "</int32>";

	if (p.io == PUP_OUT)
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

void pup(NSTextFilePUPer & p, int64 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<int64>"; endtag = "</int64>";

	if (p.io == PUP_OUT)
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

void pup(NSTextFilePUPer & p, uint8 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<uint8>"; endtag = "</uint8>";

	if (p.io == PUP_OUT)
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

void pup(NSTextFilePUPer & p, uint16 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<uint16>"; endtag = "</uint16>";

	if (p.io == PUP_OUT)
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

void pup(NSTextFilePUPer & p, uint32 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<uint32>"; endtag = "</uint32>";

	if (p.io == PUP_OUT)
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

void pup(NSTextFilePUPer & p, uint64 & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<uint64>"; endtag = "</uint64>";

	if (p.io == PUP_OUT)
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

void pup(NSTextFilePUPer & p, float & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<float>"; endtag = "</float>";

	if (p.io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		size_t beg = begtag.size(); size_t loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = std::stof(line);
	}
}

void pup(NSTextFilePUPer & p, double & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<double>"; endtag = "</double>";

	if (p.io == PUP_OUT)
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

void pup(NSTextFilePUPer & p, ldouble & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<ldouble>"; endtag = "</ldouble>";

	if (p.io == PUP_OUT)
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

void pup(NSTextFilePUPer & p, bool & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<bool>"; endtag = "</bool>";

	if (p.io == PUP_OUT)
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

