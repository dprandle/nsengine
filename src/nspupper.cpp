#include <nspupper.h>

NSFilePUPer::NSFilePUPer(PupType t_, nsfstream & fileStream, const nsuint & _io) : t(t_), fs(fileStream), io(_io) {}

NSFilePUPer::PupType NSFilePUPer::type()
{
	return t;
}

const nsuint & NSFilePUPer::mode() const
{
	return io;
}

NSBinFilePUPer::NSBinFilePUPer(nsfstream & fileStream, const nsuint & _io) :NSFilePUPer(Binary, fileStream,_io){}

void pup(NSBinFilePUPer & p, nschar & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, nswchar & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, nssint & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, nsint & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, nslint & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, nsllint & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, nsuchar & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, nsusint & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, nsuint & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, nsulint & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, nsullint & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, nsfloat & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, nsdouble & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, nsldouble & val, const nsstring & varName)
{
	pupbytes(p, val);
}

void pup(NSBinFilePUPer & p, nsbool & val, const nsstring & varName)
{
	pupbytes(p, val);
}


NSTextFilePUPer::NSTextFilePUPer(nsfstream & fileStream, const nsuint & _io) :NSFilePUPer(Text, fileStream, _io){}

void pup(NSTextFilePUPer & p, nschar & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<nschar>"; endtag = "</nschar>";

	if (p.io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		nsuint beg = begtag.size(); nsuint loc = line.find(endtag);
		line = line.substr(beg, loc-beg);
		val = line[0];
	}
}

void pup(NSTextFilePUPer & p, nswchar & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<nswchar>"; endtag = "</nswchar>";

	if (p.io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		nsuint beg = begtag.size(); nsuint loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = line[0];
	}
}

void pup(NSTextFilePUPer & p, nssint & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<nssint>"; endtag = "</nssint>";

	if (p.io == PUP_OUT)
	{
		p.fs << begtag << (nsint)val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		nsuint beg = begtag.size(); nsuint loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = (nssint)std::stoi(line);
	}
}

void pup(NSTextFilePUPer & p, nsint & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<nsint>"; endtag = "</nsint>";

	if (p.io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		nsuint beg = begtag.size(); nsuint loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = std::stoi(line);
	}
}

void pup(NSTextFilePUPer & p, nslint & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<nslint>"; endtag = "</nslint>";

	if (p.io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		nsuint beg = begtag.size(); nsuint loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = std::stol(line);
	}
}

void pup(NSTextFilePUPer & p, nsllint & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<nsllint>"; endtag = "</nsllint>";

	if (p.io == PUP_OUT)
	{
		p.fs << begtag << (nslint)val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		nsuint beg = begtag.size(); nsuint loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = (nsllint)std::stol(line);
	}
}

void pup(NSTextFilePUPer & p, nsuchar & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<nsuchar>"; endtag = "</nsuchar>";

	if (p.io == PUP_OUT)
	{
		p.fs << begtag << (nsulint)val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		nsuint beg = begtag.size(); nsuint loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = (nsuchar)std::stoul(line);
	}
}

void pup(NSTextFilePUPer & p, nsusint & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<nsusint>"; endtag = "</nsusint>";

	if (p.io == PUP_OUT)
	{
		p.fs << begtag << (nsulint)val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		nsuint beg = begtag.size(); nsuint loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = (nsusint)std::stoul(line);
	}
}

void pup(NSTextFilePUPer & p, nsuint & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<nsuint>"; endtag = "</nsuint>";

	if (p.io == PUP_OUT)
	{
		p.fs << begtag << (nsulint)val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		nsuint beg = begtag.size(); nsuint loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = (nsuint)std::stoul(line);
	}
}

void pup(NSTextFilePUPer & p, nsulint & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<nsulint>"; endtag = "</nsulint>";

	if (p.io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		nsuint beg = begtag.size(); nsuint loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = std::stoul(line);
	}
}

void pup(NSTextFilePUPer & p, nsullint & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<nsullint>"; endtag = "</nsullint>";

	if (p.io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		nsuint beg = begtag.size(); nsuint loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = std::stoull(line);
	}
}

void pup(NSTextFilePUPer & p, nsfloat & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<nsfloat>"; endtag = "</nsfloat>";

	if (p.io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		nsuint beg = begtag.size(); nsuint loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = std::stof(line);
	}
}

void pup(NSTextFilePUPer & p, nsdouble & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<nsdouble>"; endtag = "</nsdouble>";

	if (p.io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		nsuint beg = begtag.size(); nsuint loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = std::stod(line);
	}
}

void pup(NSTextFilePUPer & p, nsldouble & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<nsldouble>"; endtag = "</nsldouble>";

	if (p.io == PUP_OUT)
	{
		p.fs << begtag << val << endtag << "\n";
	}
	else
	{
		nsstring line;
		std::getline(p.fs, line);
		nsuint beg = begtag.size(); nsuint loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = std::stold(line);
	}
}

void pup(NSTextFilePUPer & p, nsbool & val, const nsstring & varName)
{
	nsstring begtag, endtag;
	begtag = varName + ":<nsbool>"; endtag = "</nsbool>";

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
		nsuint beg = begtag.size(); nsuint loc = line.find(endtag);
		line = line.substr(beg, loc - beg);
		val = (line == "true");
	}
}

