/*******************************************************************************
 * nipa
 * Updated 05/28/2011
 *
 * Nitro+ - http://www.nitroplus.co.jp/
 * Nitro+ CHiRAL - http://www.nitrochiral.com/
 * 
 * Description:
 * This is a tool made for Nitro+'s NPA archives which is used by the N2System
 * engine. The engine is used in 6 games total between Nitro+ and Nitro+ CHiRAL.
 * There are 2 versions of the NPA archive that I know of. As of v1, only the
 * most common version is supported. I plan to support the rest in the future
 * for the sake of completion.
 *
 * Documentation:
 * See http://amaterasu.is.moelicious.be/nipa/npadoc.html for detailed documentation
 * of the NPA archive format.
 *
 * Supported Games:
 * Nitro+
 *     Chaos;Head
 *     Chaos;Head Trial 1
 *     Chaos;Head Trial 2
 *     FullMetalDaemon MURAMASA
 *     FullMetalDaemon MURAMASA Janen Hen (MuramasaAD)
 *     FullMetalDaemon MURAMASA Trial
 *     Sumaga
 *     Demonbane The Best
 *     Axanael/Axanael Trial
 *     SonicomiTr1
 * Nitro+ CHiRAL
 *     Lamento -Beyond the Void-
 *     sweet pool
 *     Zoku Satsuriku no Django
 *
 * Unsupported Games:
 *     Lamento -Beyond the Void- Trial?
 *        Not tested, currently unknown.
 *        Header unencrypted in (trial was it? I can't remember)
 *
 * You're free to use this code in any way you want, as long as you you provide
 * credit in either the form of this source code or by name.
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <zlib.h>
#include "nipa.h"

int main(int argc, char **argv)
{
    int i = 0, encryption = 0, mode = 0;
    
    printf("nipa\n\n");
    
    if(argc > 1 && strlen(argv[1]) > 1 && argv[1][i++] == '-')
    {    
        while(argv[1][i] != '\0')
        {
            switch(argv[1][i++])
            {
                case 'x':
                    if(argc < 3)
                    {
                        printf("Invalid arguments for -x");
                        return 1;
                    }
                    mode = 1;
                    break;
                case 'c':
                    if(argc < 4)
                    {
                        printf("Invalid arguments for -c");
                        return 1;
                    }
                    mode = 2;
                    break;
                case 'h':
                    break;
                case 'z':
                    NPAHead.compress = 1;
                    break;
                case 'g':
                    if(argc < 4)
                    {
                        printf("Invalid arguments for -g");
                        return 1;
                    }
                    encryption = 1;
                    break;
                default:
                    printf("Unknown argument '%c', ignoring.\n",argv[1][i-1]);
                    break;
            }
        }
    }
    
    if(mode==0 || mode==3)
    {
        printf("usage:\n"\
            "General\n"\
            "-h - Help. Displays this information.\n\n"\
            "Extraction\n"\
            "-x (file) - Extract NPA archive\n"\
            "-g (id) - Game ID for encryption (if applicable). Defaults to ChaosHead if no encryption is entered\n\n"\
            "Creation\n"\
            "-c (folder) (output file) - Create archive\n"\
            "-z - Compress files\n\n"\
            "Examples\n"\
            "-x nss.npa - Extract nss.npa into the folder \"nss\"\n"\
            "-xg nss.npa MuramasaTr - Extract nss.npa into the folder \"nss\" using the Muramasa Trial encryption\n"\
            "-c nss nss.npa - Create an archive out of the folder \"nss\"\n"
            "-cz nss nss.npa - Create a compressed archive out of the folder \"nss\"\n\n"\
            "Maker - Name - ID\n"\
			"Nitro+ - Axanael - Axanael\n"\
			"Nitro+ - Axanael Trial - Axanael\n"\
            "Nitro+ - Chaos;Head - ChaosHead\n"\
            "Nitro+ - Chaos;Head Trial 1 - ChaosHeadTr1\n"\
            "Nitro+ - Chaos;Head Trial 2 - ChaosHeadTr2\n"\
            "Nitro+ - Demonbane The Best - Demonbane\n"\
            "Nitro+ - FullMetalDaemon MURAMASA - Muramasa\n"\
            "Nitro+ - FullMetalDaemon MURAMASA Janen Hen - MuramasaAD\n"\
            "Nitro+ - FullMetalDaemon MURAMASA Trial - MuramasaTr\n"\
			"Nitro+ - Kikokugai - Kikokugai\n"\
            "Nitro+ - SonicomiTr2 - SonicomiTr2\n"\
            "Nitro+ - Sumaga - Sumaga\n"\
            "Nitro+ - Sumaga Special - SumagaSP\n"\
            "Nitro+ - Zoku Satsuriku no Django - Django\n"\
            "Nitro+ - Zoku Satsuriku no Django Trial - DjangoTr *NOT WORKING*\n"\
            "Nitro+ ChiRAL - Lamento -Beyond the Void- - Lamento\n"\
            "Nitro+ ChiRAL - Lamento -Beyond the Void- Trial - LamentoTr *UNKNOWN*\n"\
            "Nitro+ ChiRAL - sweet pool - sweetpool\n",argv[0]);
            
        return 0;
    }
    
    if(encryption == 1)
    {    
        for(i = 0; games[i][0] != '\0' && stricmp(games[i],argv[argc-1]) != 0; i++)
		{
			//printf("%d %s\n",i,games[i]);
		}
        
        if(games[i][0]=='\0')
        {
            printf("Unsupported game ID: %s\n",argv[argc-1]);
            return 0;
        }
        else
        {
            NPAHead.gameid = i;
        }
    }
    
    if(mode == 1) /* Extraction mode */
    {
        parsenpa(argv[2],encryption);
    }
    else if(mode == 2) /* Creation mode */
    {
        createnpa(argc-2,argv+2); /* Get rid of the name and options */
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
    
    if(NPAHead.gameid!=8 && NPAHead.gameid!=9)
    {
        temp = NPAHead.key1 * NPAHead.key2;
    }
    else
    {
        temp = NPAHead.key1 + NPAHead.key2;
    }
        
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
	
	if(NPAHead.gameid == AXANAEL ||
		NPAHead.gameid == KIKOKUGAI ||
		NPAHead.gameid == SONICOMITR2)
		key1 = 0x20101118;
	else
		key1 = 0x87654321;
        
    for(i = 0; name[i] != 0; i++)
        key1 -= name[i];

    key  = key1 * i;
    
    if(NPAHead.gameid!=8 && NPAHead.gameid!=9) // if the game is not Lamento
    {
        key += key2;
        key *= NPAEntry[curnum].origsize;
    }
    
    return key&0xff;
}

void parsenpa(char *input, int encryption)
{
    int i = 0, x = 0;
    
    infile = fopen(input,"rb");
    if(!infile)
    {
        printf("Could not open %s",input);
        exit(1);
    }
    
    for(i = 0; input[i] != '.' && input[i] != '\0'; i++);
    input[i] = 0x00;
    
    /* Create a directory based on the input file's name then use it for output */
    mkdir(input);
    chdir(input);
    
    fread(NPAHead.head,1,7,infile);
    if(strncmp("NPA\x01",NPAHead.head,4) == 0)
    {
        printf("Parsing NPA...\n");
            
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
            printf("This is an encrypted archive. Please read the help information and select an encryption.\n");
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
            
            printf("%04d: %-50s 0x%08X [%08X]\n",i,NPAEntry[i].filename,NPAEntry[i].offset,NPAEntry[i].compsize);
            
            if(NPAEntry[i].type == 1)
            {
                mkdir(NPAEntry[i].filename);
            }
            else
            {
                extractnpa(i,ftell(infile));
            }
        }
    }
    else
    {
        printf("Invalid NPA archive\n");
    }
}

void extractnpa(int i, int pos)
{
    unsigned char *buffer = (unsigned char*)calloc(NPAEntry[i].compsize,sizeof(char));
    FILE *outfile = NULL;

    if(!buffer)
    {
        printf("Could not allocate %d bytes of memory for buffer\n",NPAEntry[i].compsize);
        exit(1);
    }
    
    outfile = fopen(NPAEntry[i].filename,"wb");    
    
    fseek(infile,NPAEntry[i].offset+NPAHead.start+0x29,SEEK_SET);    
    fread(buffer,1,NPAEntry[i].compsize,infile);
    
    if(NPAHead.encrypt == 1)
    {
        int key = crypt2(i, NPAEntry[i].filename);
        int x = 0;
        int len = 0x1000;
        
        if(NPAHead.gameid!=8 || NPAHead.gameid==9)
            len += strlen(NPAEntry[i].filename);
        
        for(x = 0; x < NPAEntry[i].compsize && x < len; x++)
        {
			if(NPAHead.gameid==8 || NPAHead.gameid==9)
				buffer[x] = keytbl[NPAHead.gameid][buffer[x]]-key;
			else
				buffer[x] = (keytbl[NPAHead.gameid][buffer[x]]-key)-x;
		}
    }
    
    if(NPAHead.compress == 1)
    {
        char *zbuffer = (char*)calloc(NPAEntry[i].origsize,sizeof(char));
        
        uncompress(zbuffer,&NPAEntry[i].origsize,buffer,NPAEntry[i].compsize);
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

void createnpa(int count, char **inarr)
{
    int i = 0, x = 0;
    
    strcat(origpath,inarr[0]);
    strcat(origpath,"\\*");
    
    printf("Parsing directory structure, this could take a moment...\n");
    parsedir(origpath);
    
    /* Prepare the struct */
    strcat(NPAHead.head,"NPA\x01\x00\x00\x00");
    
    NPAHead.key1 = 0x4147414E;
    NPAHead.key2 = 0x21214F54;
    NPAHead.encrypt = 0;
    NPAHead.filecount = (NPAHead.totalcount-NPAHead.foldercount);
    NPAHead.start += NPAHead.totalcount*0x15; /* DO NOT CHANGE */
    NPAEntry[0].offset = 0; /* Must initialize the first offset or it'll fuck everything up. */
    
    outfile = fopen(inarr[1],"wb+");
    
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
        char *encname = (char*)calloc(NPAEntry[i].nlength,sizeof(char));
        
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
        
        free(encname);
    }
    
    /* Write the file data */
    for(i = 0; i < NPAHead.totalcount; i++)
    {
        if(NPAEntry[i].type == 2) /* File */
        {
            char tempstr[MAX_PATH];
                        
            printf("%04d: %-50s 0x%08X [%08X]\n",i,NPAEntry[i].filename,NPAEntry[i].offset,NPAEntry[i].origsize);
            sprintf(tempstr,"%s%s",origpath,NPAEntry[i].filename); /* Prepare the file location string */
            
            infile = fopen(tempstr,"rb");
            if(infile)
            {
                char *buffer = (char*)calloc(NPAEntry[i].origsize,sizeof(char));
                
                fread(buffer,1,NPAEntry[i].origsize,infile);
                
                if(NPAHead.compress == 1)
                {
                    char *zbuffer = (char*)calloc(NPAEntry[i].origsize,sizeof(char)); /* Second buffer for zlib */
                
                    compress(zbuffer,&NPAEntry[i].compsize,buffer,NPAEntry[i].origsize);
                    fwrite(zbuffer,1,NPAEntry[i].compsize,outfile); /* I feel there might be a better way to do this... */
                    
                    free(zbuffer);
                }
                else
                {
                    fwrite(buffer,1,NPAEntry[i].origsize,outfile);
                }
                    
                fclose(infile);
                free(buffer);
            }
            else
            {
                printf("Could not open %s\n",tempstr);
            }
        }
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
void addentry(char *path, char *name, int id, int type, int subdir)
{
    char *temp, *temp2;
    int i = 0;
    FILE *tempf;
    
    temp = (char*)calloc(strlen(path)+1,sizeof(char));    
    temp2 = (char*)calloc(strlen(path)+strlen(name)+1,sizeof(char));    
    NPAEntry = realloc(NPAEntry,(NPAHead.totalcount+1)*0x1c); /* Don't remove this +1 here, it causes bad things to happen as I found out at 6 in the morning */
    NPAEntry[NPAHead.totalcount].filename = calloc(strlen(name)+strlen(path)+1,sizeof(char));
    NPAEntry[NPAHead.totalcount].compsize = 0;
    NPAEntry[NPAHead.totalcount].origsize = 0;
    
    if(path[strlen(path)-1] == '*')
    {
        path[strlen(path)-1] = 0x00; /* Strip the trailing wildcard if it exists, we don't want this in our table */
    }
    
    /*
     * Now, remove the original path since it can't be in the archive's table
     * Probably a noobish way to do this but whatever
     */
    for(i = 0; i < strlen(path)-strlen(origpath); i++)
    {
        temp[i] = path[strlen(origpath)+i];
    }
    
    strcat(NPAEntry[NPAHead.totalcount].filename,temp);
    strcat(NPAEntry[NPAHead.totalcount].filename,name);
    
    NPAHead.start += strlen(NPAEntry[NPAHead.totalcount].filename);
    NPAEntry[NPAHead.totalcount].nlength = strlen(NPAEntry[NPAHead.totalcount].filename);    
    NPAEntry[NPAHead.totalcount].fileid = id;
    
    strcat(temp2,path);
    strcat(temp2,name);

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
        tempf = fopen(temp2,"rb");
        
        if(!temp)
        {
            printf("Could not open %s\n",temp2); /* This should not occur unless the file somehow goes missing between finding it in the directory and being checked here. */
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
}

void parsedir(char *path)
{
    /* Win32 API ftl MSDN to the rescue at least */
    WIN32_FIND_DATA fd;
    HANDLE handle;
    int curid=id++;
        
    handle = FindFirstFile(path,&fd);
    do 
    {
        if(strcmp(fd.cFileName,".") != 0 && strcmp(fd.cFileName,"..") != 0 && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)) /* Skip cur dir, prev dir, and any hidden files */
        {                
            if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                subdir++;
            }
            
            addentry(path,fd.cFileName,curid,fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY,subdir);
            
            if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                char *name = (char*)calloc(MAX_PATH,sizeof(char));
                
                strcat(name,path);
                strcat(name,fd.cFileName);
                strcat(name,"\\*");
                
                NPAHead.foldercount++;
                parsedir(name);
                
                subdir--;
                
                free(name);
            }
        }
    } while(FindNextFile(handle,&fd) != 0);
}
