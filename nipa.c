/*******************************************************************************
* nipa
* Updated 05/28/2011
*
* You're free to use this code in any way you want, as long as you you provide
* credit in either the form of this source code or by name.
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include "zlib/zlib.h"
#include "nipa.h"

NPAHEAD NPAHead;
NPAENTRY *NPAEntry;

int offset = 0, subdir = 0, id = 0;
TCHAR origpath[MAX_PATH];

FILE *infile,*outfile;

enum { MODE_HELP, MODE_EXTRACT, MODE_CREATE };
int _tmain(int argc, TCHAR **argv)
{
	int i = 0, encryption = 0, mode = MODE_HELP;

#ifdef _UNICODE
	_setmode(_fileno(stdout), _O_WTEXT);
#endif
	_tprintf(_T("nipa\n\n"));

	if(argc > 1 && _tcslen(argv[1]) > 1 && argv[1][i++] == _T('-'))
	{    
		while(argv[1][i] != _T('\0'))
		{
			switch(argv[1][i++])
			{
			case _T('x'):
				if(argc < 3)
				{
					_tprintf(_T("Invalid arguments for -x"));
					return 1;
				}
				mode = MODE_EXTRACT;
				break;
			case _T('c'):
				if ( encryption ) {
					if(argc < 5)
					{
						_tprintf(_T("Invalid arguments for -gc"));
						return 1;
					}
				} else {
					if(argc < 4)
					{
						_tprintf(_T("Invalid arguments for -c"));
						return 1;
					}
				}
				mode = MODE_CREATE;
				break;
			case _T('h'):
				mode = MODE_HELP;
				break;
			case _T('z'):
				NPAHead.compress = 1;
				break;
			case _T('g'):
				if ( mode == MODE_CREATE ) {
					if(argc < 5)
					{
						_tprintf(_T("Invalid arguments for -cg"));
						return 1;
					}
				} else {
					if(argc < 4)
					{
						_tprintf(_T("Invalid arguments for -g"));
						return 1;
					}
				}
				encryption = 1;
				break;
			default:
				_tprintf(_T("Unknown argument '%c', ignoring.\n",argv[1][i-1]));
				break;
			}
		}
	}

	if(mode==MODE_HELP)
	{
		_tprintf(_T("usage:\n")
			_T("General\n")
			_T("\t-h - Help. Displays this information.\n\n")
			_T("Extraction\n")
			_T("\t-x (file) - Extract NPA archive\n")
			_T("\t-g (id) - Game ID for encryption (if applicable). Defaults to ChaosHead if no encryption is entered\n\n")
			_T("Creation\n")
			_T("\t-c (folder) (output file) - Create archive\n")
			_T("\t-z - Compress files\n\n")
			_T("Examples\n")
			_T("\t-x nss.npa - Extract nss.npa into the folder \"nss\"\n")
			_T("\t-xg nss.npa MuramasaTr - Extract nss.npa into the folder \"nss\" using the Muramasa Trial encryption\n")
			_T("\t-c nss nss.npa - Create an archive out of the folder \"nss\"\n")
			_T("\t-cz nss nss.npa - Create a compressed archive out of the folder \"nss\"\n\n")
			_T("Maker - Name - ID\n")
			_T("Nitro+ - Axanael - Axanael\n")
			_T("Nitro+ - Axanael Trial - Axanael\n")
			_T("Nitro+ - Chaos;Head - ChaosHead\n")
			_T("Nitro+ - Chaos;Head Trial 1 - ChaosHeadTr1\n")
			_T("Nitro+ - Chaos;Head Trial 2 - ChaosHeadTr2\n")
			_T("Nitro+ - Demonbane The Best - Demonbane\n")
			_T("Nitro+ - FullMetalDaemon MURAMASA - Muramasa\n")
			_T("Nitro+ - FullMetalDaemon MURAMASA Janen Hen - MuramasaAD\n")
			_T("Nitro+ - FullMetalDaemon MURAMASA Trial - MuramasaTr\n")
			_T("Nitro+ - FullMetalDaemon MURAMASA Shokuzai hen - MuramasaSS\n")
			_T("Nitro+ - Guilty Crown Lost Xmas - LostX\n")
			_T("Nitro+ - Guilty Crown Lost Xmas Trailer - LostXTrailer\n")
			_T("Nitro+ - Kikokugai - Kikokugai\n")
			_T("Nitro+ - Sonicomi - Sonicomi\n")
			_T("Nitro+ - Sonicomi Trial 2 - SonicomiTr2\n")
			_T("Nitro+ - Sumaga - Sumaga\n")
			_T("Nitro+ - Sumaga3P - Sumaga 3%% Trial\n")
			_T("Nitro+ - Sumaga Special - SumagaSP\n")
			_T("Nitro+ - Kimi to Kajono to Kanojo no Koi - Totono\n")
			_T("Nitro+ - Zoku Satsuriku no Django - Django\n")
			_T("Nitro+ - Zoku Satsuriku no Django Trial - DjangoTr *NOT WORKING*\n")
			_T("Nitro+ ChiRAL - Lamento -Beyond the Void- - Lamento\n")
			_T("Nitro+ ChiRAL - Lamento -Beyond the Void- Trial - LamentoTr *UNKNOWN*\n")
			_T("Nitro+ ChiRAL - sweet pool - sweetpool\n"),
			_T("Nitro+ ChiRAL - Dramatical Murder - DramaticalMurder\n"),
			_T("Nitro+ ChiRAL - DRAMAtical Murder re:connect - DramaticalMurderRC\n"),
			argv[0]);

		return 0;
	}

	if(encryption == 1)
	{    
		for(i = 0; games[i][0] != _T('\0') && _tcsicmp(games[i],argv[argc-1]) != 0; i++);

		if(games[i][0]==_T('\0'))
		{
			_tprintf(_T("Unsupported game ID: %s\n"),argv[argc-1]);
			return 0;
		}
		else
		{
			NPAHead.gameid = i;
		}
	}

	if(mode == MODE_EXTRACT)
	{
		parsenpa(argv[2],encryption);
	}
	else if(mode == MODE_CREATE)
	{
		createnpa(argc-2,argv+2,encryption); /* Get rid of the name and options */
	}

	return 0;
}

/*
* This is the algorithm function for the header data.
* It's a symmetric algorithm.
*
* To decrypt: Add the key and byte together.
* To encrypt: Subtract the key and byte.
*
* DO NOT TOUCH or the loli will bite you (and not in a sexual way).
*/
int crypt(int curnum, int curfile)
{
	int key = 0xFC*curnum;
	int temp = 0;

	if ((NPAHead.gameid==LAMENTO && NPAHead.encrypt) || (NPAHead.gameid==LAMENTOTR && NPAHead.encrypt))
		temp = NPAHead.key1 + NPAHead.key2;
	else
		temp = NPAHead.key1 * NPAHead.key2;

	key -= temp >> 0x18;
	key -= temp >> 0x10;
	key -= temp >> 0x08;
	key -= temp  & 0xff;

	key -= curfile >> 0x18;
	key -= curfile >> 0x10;
	key -= curfile >> 0x08;
	key -= curfile;

	return key & 0xff;
}

/*
* The algorithm for the file data.
* I haven't had a reason to test encrypting with this function, but I assume it's symmetric too.
*
* For the love of lolis, do not touch this again or you'll fuck it up again and have to rewrite it.
*
* If you get an integer overflow when compiling the source, it's because of this function (in particular, key2).
* This is normal and the program requires it. Change it and you'll anger the lolis.
*/
int crypt2(int curnum, char *name)
{
	int i=0;
	int key1 = 0; /* 2345678 hurr */
	int key2 = NPAHead.key1 * NPAHead.key2;
	int key = 0;

	switch(NPAHead.gameid) {
		case AXANAEL:
		case KIKOKUGAI:
		case SONICOMITR2:
		case SONICOMI:
		case LOSTX:
		case DRAMATICALMURDER:
		case DRAMATICALMURDERRC:
		case MURAMASASS:
			key1 = 0x20101118;
			break;
		case TOTONO:
			key1 = 0x12345678;
			break;
		default:
			key1 = 0x87654321;
			break;
	}

	for(i = 0; name[i] != 0; i++)
		key1 -= name[i];

	key  = key1 * i;

	if(NPAHead.gameid!=LAMENTO && NPAHead.gameid!=LAMENTOTR) // if the game is not Lamento
	{
		key += key2;
		key *= NPAEntry[curnum].origsize;
	}

	return key&0xff;
}

void parsenpa(TCHAR *input, int encryption)
{
	int i = 0, x = 0;

	infile = _tfopen(input,_T("rb"));
	if(!infile)
	{
		_tprintf(_T("Could not open %s"),input);
		exit(1);
	}

	for(i = 0; input[i] != _T('.') && input[i] != _T('\0'); i++);
	input[i] = 0x00;

	/* Create a directory based on the input file's name then use it for output */
	_tmkdir(input);
	_tchdir(input);

	fread(NPAHead.head,1,7,infile);
	if(strncmp("NPA\x01",NPAHead.head,4) == 0)
	{
		_tprintf(_T("Parsing NPA...\n"));

		fread(&NPAHead.key1,1,4,infile);
		fread(&NPAHead.key2,1,4,infile);
		fread(&NPAHead.compress,1,1,infile);
		fread(&NPAHead.encrypt,1,1,infile);
		fread(&NPAHead.totalcount,1,4,infile);
		fread(&NPAHead.foldercount,1,4,infile);
		fread(&NPAHead.filecount,1,4,infile);
		fread(&NPAHead.null,1,8,infile);
		fread(&NPAHead.start,1,4,infile);

		if(NPAHead.encrypt == 1 && encryption == 0) /* Late night, thought I broke something. Turns out I didn't set an encryption. Fuck. A check so people won't make the same mistake. */
		{
			_tprintf(_T("This is an encrypted archive. Please read the help information and select an encryption.\n"));
			exit(1);
		}

		NPAEntry = calloc(NPAHead.totalcount,sizeof(*NPAEntry));

		for(i = 0; i < NPAHead.totalcount; i++)
		{
			fread(&NPAEntry[i].nlength,1,4,infile);

			NPAEntry[i].filename = (char*)calloc(NPAEntry[i].nlength+1,sizeof(char));
			fread(NPAEntry[i].filename,1,NPAEntry[i].nlength,infile);

			for(x = 0; x < NPAEntry[i].nlength; x++)
				NPAEntry[i].filename[x] += crypt(x,i);

			NPAEntry[i].filename[x] = '\0'; // when i compiled nipa using cl and ran it on win7, it wasn't properly terminating the string

			fread(&NPAEntry[i].type,1,1,infile);
			fread(&NPAEntry[i].fileid,1,4,infile);
			fread(&NPAEntry[i].offset,1,4,infile);
			fread(&NPAEntry[i].compsize,1,4,infile);
			fread(&NPAEntry[i].origsize,1,4,infile);

			{
#if _UNICODE
				TCHAR destinationPath[MAX_PATH];
				MultiByteToWideChar(CODEPAGE_SHIFT_JIS,0,NPAEntry[i].filename,-1,destinationPath,MAX_PATH);
#else
				char *destinationPath = NPAEntry[i].filename;
#endif
				_tprintf(_T("%04d: %-50s 0x%08X [%08X]\n"),i,destinationPath,NPAEntry[i].offset,NPAEntry[i].compsize);

				if(NPAEntry[i].type == 1)
				{
					_tmkdir(destinationPath);
				}
				else
				{
					extractnpa(i,ftell(infile), destinationPath);
				}
			}
		}
	}
	else
	{
		_tprintf(_T("Invalid NPA archive\n"));
	}
}

void extractnpa(int i, int pos, TCHAR *destination)
{
	unsigned char *buffer = (unsigned char*)calloc(NPAEntry[i].compsize,sizeof(char));
	FILE *outfile = NULL;

	if(!buffer)
	{
		_tprintf(_T("Could not allocate %d bytes of memory for buffer\n"),NPAEntry[i].compsize);
		exit(1);
	}

	outfile = _tfopen(destination,_T("wb")); 
	if ( !outfile ) {
		_tprintf(_T("--WARNING: Cannot write file. %s"),_tcserror(errno));
		return;
	}
	fseek(infile,NPAEntry[i].offset+NPAHead.start+0x29,SEEK_SET);    
	fread(buffer,1,NPAEntry[i].compsize,infile);

	if(NPAHead.encrypt == 1)
	{
		int key = crypt2(i, NPAEntry[i].filename);
		int x = 0;
		int len = 0x1000;

		if(NPAHead.gameid!=LAMENTO && NPAHead.gameid!=LAMENTOTR)
			len += strlen(NPAEntry[i].filename);

		for(x = 0; x < NPAEntry[i].compsize && x < len; x++)
		{
			if(NPAHead.gameid==LAMENTO || NPAHead.gameid==LAMENTOTR) {
				buffer[x] = keytbl[NPAHead.gameid][buffer[x]]-key;
			} else if(NPAHead.gameid==TOTONO) {
				unsigned char r = buffer[x];
				char r2;
				r = keytbl[NPAHead.gameid][r];
				r = keytbl[NPAHead.gameid][r];
				r = keytbl[NPAHead.gameid][r];
				r = ~r;
				r2 = (char)r - key - x;
				buffer[x] = (unsigned char) r2;
			} else {
				buffer[x] = (keytbl[NPAHead.gameid][buffer[x]]-key)-x;
			}
		}
	}

	if(NPAHead.compress == 1)
	{
		char *zbuffer = (char*)calloc(NPAEntry[i].origsize,sizeof(char));

		if(uncompress(zbuffer,&NPAEntry[i].origsize,buffer,NPAEntry[i].compsize) != Z_OK) {
			_tprintf(_T("Uncompress failed!\n"));
			exit(1);
		}
		fwrite(zbuffer,1,NPAEntry[i].origsize,outfile);

		free(zbuffer);
	}
	else
	{
		fwrite(buffer,1,NPAEntry[i].origsize,outfile);
	}

	free(buffer);
	fclose(outfile);

	fseek(infile,pos,SEEK_SET);
}

void createnpa(int count, TCHAR **inarr, int encrypt)
{
	int i = 0, x = 0;
	char encname[MAX_PATH];
	unsigned char encTable[0x100];

	_tcscat(origpath,inarr[0]);
	_tcscat(origpath,_T("\\*"));

	_tprintf(_T("Parsing directory structure, this could take a moment...\n"));
	parsedir(origpath);

	/* Prepare the struct */
	strcat(NPAHead.head,"NPA\x01\x00\x00\x00");

	NPAHead.key1 = 0x4147414E;
	NPAHead.key2 = 0x21214f54;
	NPAHead.encrypt = encrypt;
	if ( encrypt ) {
		size_t i;
		//build reverse key table for encryption
		for (i = 0; i < sizeof(encTable)/sizeof(encTable[0]); ++i)
			encTable[keytbl[NPAHead.gameid][i]] = i;
	}
	NPAHead.filecount = (NPAHead.totalcount-NPAHead.foldercount);
	NPAHead.start += NPAHead.totalcount*0x15; /* DO NOT CHANGE */
	NPAEntry[0].offset = 0; /* Must initialize the first offset or it'll fuck everything up. */

	outfile = _tfopen(inarr[1],_T("wb"));

	/* Write the header */
	fwrite(NPAHead.head,1,7,outfile);
	fwrite(&NPAHead.key1,1,4,outfile);
	fwrite(&NPAHead.key2,1,4,outfile);
	fwrite(&NPAHead.compress,1,1,outfile);
	fwrite(&NPAHead.encrypt,1,1,outfile);
	fwrite(&NPAHead.totalcount,1,4,outfile);
	fwrite(&NPAHead.foldercount,1,4,outfile);
	fwrite(&NPAHead.filecount,1,4,outfile);
	fwrite("\x00\x23\x65\x6E\x73\x75\x65\x00",1,8,outfile);
	fwrite(&NPAHead.start,1,4,outfile);

	/* Write the file table */
	for(i = 0; i < NPAHead.totalcount; i++)
	{
		for(x = 0; x < NPAEntry[i].nlength; x++)
		{
			encname[x] = NPAEntry[i].filename[x] - crypt(x,i);
		}

		fwrite(&NPAEntry[i].nlength,1,4,outfile);
		fwrite(encname,1,NPAEntry[i].nlength,outfile);
		fwrite(&NPAEntry[i].type,1,1,outfile);
		fwrite(&NPAEntry[i].fileid,1,4,outfile);
		fwrite(&NPAEntry[i].offset,1,4,outfile);
		fwrite(&NPAEntry[i].compsize,1,4,outfile);
		fwrite(&NPAEntry[i].origsize,1,4,outfile);
	}

	{
		int bufferSz = 0, zbufferSz = 0;
		Byte *buffer = 0, *zbuffer = 0; /* Second buffer for zlib */
		const int origpathLen = _tcslen(origpath);
		/* Write the file data */
		for(i = 0; i < NPAHead.totalcount; i++)
		{
			if(NPAEntry[i].type == 2) /* File */
			{
				/* Prepare the file location string */
				TCHAR tempstr[MAX_PATH];
				_tcscpy(tempstr, origpath);
#ifdef _UNICODE
				MultiByteToWideChar(CODEPAGE_SHIFT_JIS,0,NPAEntry[i].filename,NPAEntry[i].nlength+1,tempstr+origpathLen,MAX_PATH-origpathLen);
#else
				_tcscat(tempstr,NPAEntry[i].filename);
#endif
				_tprintf(_T("%04d: %-50s 0x%08X [%08X]\n"),i,tempstr+origpathLen,NPAEntry[i].offset,NPAEntry[i].origsize);

				infile = _tfopen(tempstr,_T("rb"));
				if(infile)
				{
					Byte *writeSrc = 0;
					int writeSrcBytes = 0;

					if ( bufferSz < NPAEntry[i].origsize ) {
						free(buffer);
						buffer = (Byte*)calloc(NPAEntry[i].origsize,sizeof(Byte));
						bufferSz = NPAEntry[i].origsize;
					}
					
					fread(buffer,1,NPAEntry[i].origsize,infile);

					if(NPAHead.compress == 1)
					{
						if ( zbufferSz < NPAEntry[i].origsize ) {
							free(zbuffer);
							zbuffer = (Byte*)calloc(NPAEntry[i].origsize,sizeof(Byte));
							zbufferSz = NPAEntry[i].origsize;
						}
						compress(zbuffer,&NPAEntry[i].compsize,buffer,NPAEntry[i].origsize);
						writeSrc = zbuffer;
						writeSrcBytes = NPAEntry[i].compsize;
					}
					else
					{
						writeSrc = buffer;
						writeSrcBytes = NPAEntry[i].origsize;
					}
					
					if ( NPAHead.encrypt ) {
						int key = crypt2(i, NPAEntry[i].filename);
						int x = 0;
						int len = 0x1000;

						if(NPAHead.gameid!=LAMENTO && NPAHead.gameid!=LAMENTOTR)
							len += strlen(NPAEntry[i].filename);

						for(x = 0; x < writeSrcBytes && x < len; x++)
						{
							if(NPAHead.gameid==LAMENTO || NPAHead.gameid==LAMENTOTR) {
								writeSrc[x] = encTable[(writeSrc[x] + key) & 0xFF];
							} else if(NPAHead.gameid==TOTONO) {
								char r = writeSrc[x] + x + key;
								unsigned char r2;
								r = ~r;
								r2 = (unsigned char) r;
								r2 = encTable[r2];
								r2 = encTable[r2];
								r2 = encTable[r2];
								writeSrc[x] = r2;
							} else {
								writeSrc[x] = encTable[(writeSrc[x] + x + key) & 0xFF];
							}
						}
					}
					fwrite(writeSrc,1,writeSrcBytes,outfile);

					fclose(infile);
				}
				else
				{
					_tprintf(_T("Could not open %s\n"),tempstr);
				}
			}
		}
		free(buffer);
		free(zbuffer);
	}

	/*
	* Fix the header if it's a zlib'd file.
	* I can't think of any other way to write the correct sizes except for this.
	*/
	if(NPAHead.compress == 1)
	{
		fseek(outfile,0x29,SEEK_SET);

		for(i = 0; i < NPAHead.totalcount; i++)
		{
			if(i != 0) /* Need to have this else it'll try reading NPAEntry[-1] to make the starting offset */
			{
				NPAEntry[i].offset = NPAEntry[i-1].offset+NPAEntry[i-1].compsize;
			}

			fseek(outfile,NPAEntry[i].nlength+9,SEEK_CUR);
			fwrite(&NPAEntry[i].offset,1,4,outfile);
			fwrite(&NPAEntry[i].compsize,1,4,outfile);
			fwrite(&NPAEntry[i].origsize,1,4,outfile);
		}
	}

	fclose(outfile);
}

/*
* More trouble than it should've been... might be a cause of future problems.
* QUALITY ENDS HERE!
* Don't look. Seriously.
*/
void addentry(TCHAR *path, TCHAR *name, int id, int type, int subdir)
{
	TCHAR *relativeDir, *temp2;
	int i = 0;
	FILE *tempf;
	int pathLen = _tcslen(path);
	temp2 = (TCHAR*)calloc(pathLen+_tcslen(name)+1,sizeof(TCHAR));
	NPAEntry = realloc(NPAEntry,(NPAHead.totalcount+1)*0x1c); /* Don't remove this +1 here, it causes bad things to happen as I found out at 6 in the morning */
	NPAEntry[NPAHead.totalcount].compsize = 0;
	NPAEntry[NPAHead.totalcount].origsize = 0;

	if(path[pathLen-1] == '*')
	{
		path[pathLen-1] = 0x00; /* Strip the trailing wildcard if it exists, we don't want this in our table */
		--pathLen;
	}

	/*
	* Now, remove the original path since it can't be in the archive's table
	* Probably a noobish way to do this but whatever
	*/
	relativeDir = path + _tcslen(origpath);
	{
		const int relativePathSz = _tcslen(relativeDir) + _tcslen(name) + 1;
#ifdef _UNICODE
		TCHAR relativePathBuffer[MAX_PATH];
		BOOL usedDefault = FALSE;
		int sz;

		_tcscpy(relativePathBuffer,relativeDir);
		_tcscat(relativePathBuffer,name);

		sz = WideCharToMultiByte(CODEPAGE_SHIFT_JIS,0,relativePathBuffer,relativePathSz,NULL,0,NULL,&usedDefault);
		NPAEntry[NPAHead.totalcount].filename = (char*)calloc(sz, sizeof(char));
		WideCharToMultiByte(CODEPAGE_SHIFT_JIS,0,relativePathBuffer,relativePathSz,NPAEntry[NPAHead.totalcount].filename,sz,NULL,NULL);
#else
		NPAEntry[NPAHead.totalcount].filename = (char*)calloc(relativePathSz, sizeof(char));
		_tcscat(NPAEntry[NPAHead.totalcount].filename,relativeDir);
		_tcscat(NPAEntry[NPAHead.totalcount].filename,name);
#endif
	}

	_tcscpy(temp2,path);
	_tcscat(temp2,name);

	NPAEntry[NPAHead.totalcount].nlength = strlen(NPAEntry[NPAHead.totalcount].filename);    
	NPAHead.start += NPAEntry[NPAHead.totalcount].nlength;
	NPAEntry[NPAHead.totalcount].fileid = id;


	if(type != 0)
	{
		NPAEntry[NPAHead.totalcount].type = 1; /* Folders and any other possible type value gets put as type 2 */
	}
	else 
	{
		NPAEntry[NPAHead.totalcount].type = 2; /* File */
	}

	if(NPAEntry[NPAHead.totalcount].type == 2)
	{
		tempf = _tfopen(temp2,_T("rb"));

		if(!tempf)
		{
			_tprintf(_T("Could not open %s\n"),temp2); /* This should not occur unless the file somehow goes missing between finding it in the directory and being checked here. */
			exit(1); /* In that case, something went horribly wrong. */
		}

		fseek(tempf,0,SEEK_END);

		NPAEntry[NPAHead.totalcount].compsize = ftell(tempf);
		NPAEntry[NPAHead.totalcount].origsize = NPAEntry[NPAHead.totalcount].compsize; /* Gets changed later if compression is enabled. */

		fclose(tempf);
	}

	if(NPAHead.totalcount > 0) /* Generate the offset, only valid for uncompressed files */
	{
		offset += NPAEntry[NPAHead.totalcount-1].compsize;
		NPAEntry[NPAHead.totalcount].offset = offset;
	}

	if(subdir>1)
	{
		NPAEntry[NPAHead.totalcount].fileid = ++id; /* Consider each folder to be a file as well. root = 0, CG = 1, Sound = 2, etc. CG\image.jpg = 3 CG\image2.jpg = 3, and so on. CG\Images\ = 3 also but CG\Images\file.jpg = 4. Nitro+ can burn in hell for this. */
	}

	NPAHead.totalcount++;
	free(temp2);
}

void parsedir(TCHAR *path)
{
	/* Win32 API ftl MSDN to the rescue at least */
	WIN32_FIND_DATA fd;
	HANDLE handle;
	int curid=id++;

	handle = FindFirstFile(path,&fd);
	do 
	{
		if(_tcscmp(fd.cFileName,_T(".")) != 0 && _tcscmp(fd.cFileName,_T("..")) != 0 && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)) /* Skip cur dir, prev dir, and any hidden files */
		{                
			if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				subdir++;
			}

			addentry(path,fd.cFileName,curid,fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY,subdir);

			if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				TCHAR *name = (TCHAR*)calloc(MAX_PATH,sizeof(TCHAR));

				_tcscat(name,path);
				_tcscat(name,fd.cFileName);
				_tcscat(name,_T("\\*"));

				NPAHead.foldercount++;
				parsedir(name);

				subdir--;

				free(name);
			}
		}
	} while(FindNextFile(handle,&fd) != 0);
}
