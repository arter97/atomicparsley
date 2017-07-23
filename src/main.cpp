//==================================================================//
/*
    AtomicParsley - main.cpp

    AtomicParsley is GPL software; you can freely distribute,
    redistribute, modify & use under the terms of the GNU General
    Public License; either version 2 or its successor.

    AtomicParsley is distributed under the GPL "AS IS", without
    any warranty; without the implied warranty of merchantability
    or fitness for either an expressed or implied particular purpose.

    Please see the included GNU General Public License (GPL) for
    your rights and further details; see the file COPYING. If you
    cannot, write to the Free Software Foundation, 59 Temple Place
    Suite 330, Boston, MA 02111-1307, USA.  Or www.fsf.org

    Copyright ©2005-2007 puck_lock
    with contributions from others; see the CREDITS file

    ----------------------
    Code Contributions by:

    * Mike Brancato - Debian patches & build support
    * Brian Story - porting getopt & native Win32 patches
                                                                   */
//==================================================================//

#include "AtomicParsley.h"

// define one-letter cli options for
#define OPT_HELP                 'h'
#define OPT_TEST                 'T'
#define OPT_ShowTextData         't'
#define OPT_ExtractPix           'E'
#define OPT_ExtractPixToPath     'e'
#define Meta_artist              'a'
#define Meta_artDirector         0xC6
#define Meta_arranger            0xC8
#define Meta_author              0xC9
#define Meta_conductor           0xD0
#define Meta_director            0xD1
#define Meta_originalArtist      0xD2
#define Meta_producer            0xD3
//#define Meta_performer           0xD4
#define Meta_soundEngineer       0xD5
#define Meta_soloist             0xD6
#define Meta_executiveProducer   0xD7
#define Meta_songtitle           's'
#define Meta_subtitle            0xC5
#define Meta_album               'b'
#define Meta_tracknum            'k'
#define Meta_disknum             'd'
#define Meta_genre               'g'
#define Meta_comment             'c'
#define Meta_year                'y'
#define Meta_lyrics              'l'
#define Meta_lyrics_file         0xC7
#define Meta_composer            'w'
#define Meta_copyright           'x'
#define Meta_grouping            'G'
#define Meta_album_artist        'A'
#define Meta_compilation         'C'
#define Meta_hdvideo             'O'
#define Meta_BPM                 'B'
#define Meta_artwork             'r'
#define Meta_advisory            'V'
#define Meta_stik                'S'
#define Meta_description         'p'
#define Meta_Rating              0xCB
#define Meta_longdescription     'j'
#define Meta_TV_Network          'n'
#define Meta_TV_ShowName         'H'
#define Meta_TV_EpisodeNumber    'N'
#define Meta_TV_SeasonNumber     'U'
#define Meta_TV_Episode          'I'
#define Meta_podcastFlag         'f'
#define Meta_category            'q'
#define Meta_keyword             'K'
#define Meta_podcast_URL         'L'
#define Meta_podcast_GUID        'J'
#define Meta_PurchaseDate        'D'
#define Meta_apID                'Y'
#define Meta_cnID                0xC0
#define Meta_geID                0xC2
#define Meta_xID                 0xC3
#define Meta_storedescription    0xC4
#define Meta_EncodingTool        0xB7
#define Meta_EncodedBy           0xC1
#define Meta_PlayGapless         0xBA
#define Meta_SortOrder           0xBF

#define Meta_ReverseDNS_Form     'M'
#define Meta_rDNS_rating         0xBB

#define Meta_StandardDate        'Z'
#define Meta_URL                 'u'
#define Meta_Information         'i'
#define Meta_uuid                'z'
#define Opt_Extract_all_uuids    0xB8
#define Opt_Extract_a_uuid       0xB9
#define Opt_Ipod_AVC_uuid        0xBE

#define Metadata_Purge           'P'
#define UserData_Purge           'X'
#define foobar_purge             '.'
#define Meta_dump                'Q'
#define Manual_atom_removal      'R'
#define Opt_FreeFree             'F'
#define OPT_OutputFile           'o'
#define OPT_NoOptimize           0xBD

#define OPT_OverWrite            'W'

#if defined (_WIN32)
#define OPT_PreserveTimeStamps   0xCA
#endif

#define ISO_Copyright            0xAA

#define _3GP_Title               0xAB
#define _3GP_Author              0xAC
#define _3GP_Performer           0xAD
#define _3GP_Genre               0xAE
#define _3GP_Description         0xAF
#define _3GP_Copyright           0xB0
#define _3GP_Album               0xB1
#define _3GP_Year                0xB2
#define _3GP_Rating              0xB3
#define _3GP_Classification      0xB4
#define _3GP_Keyword             0xB5
#define _3GP_Location            0xB6

#define Meta_ID3v2Tag            0xBC

char *output_file;

int total_args;

static void kill_signal ( int sig );

static void kill_signal (int sig) {
    exit(0);
}

static const char* shortHelp_text = "\n";
static const char* longHelp_text = "\n";
static const char* fileLevelHelp_text = "\n";
static const char* _3gpHelp_text = "\n";
static const char* ISOHelp_text = "\n";
static const char* uuidHelp_text = "\n";
static const char* rDNSHelp_text = "\n";
static const char* ID3Help_text = "\n";

void ExtractPaddingPrefs(char* env_padding_prefs) {
    pad_prefs.default_padding_size = DEFAULT_PADDING_LENGTH;
    pad_prefs.minimum_required_padding_size = MINIMUM_REQUIRED_PADDING_LENGTH;
    pad_prefs.maximum_present_padding_size = MAXIMUM_REQUIRED_PADDING_LENGTH;

    if (env_padding_prefs != NULL) {
        if (env_padding_prefs[0] == 0x22 || env_padding_prefs[0] == 0x27) env_padding_prefs++;
    }
    char* env_pad_prefs_ptr = env_padding_prefs;

    while (env_pad_prefs_ptr != NULL) {
        env_pad_prefs_ptr = strsep(&env_padding_prefs,":");

        if (env_pad_prefs_ptr == NULL) break;

        if (strncmp(env_pad_prefs_ptr, "DEFAULT_PAD=", 12) == 0) {
            strsep(&env_pad_prefs_ptr,"=");
            sscanf(env_pad_prefs_ptr, "%" SCNu32, &pad_prefs.default_padding_size);
        }
        if (strncmp(env_pad_prefs_ptr, "MIN_PAD=", 8) == 0) {
            strsep(&env_pad_prefs_ptr,"=");
            sscanf(env_pad_prefs_ptr, "%" SCNu32, &pad_prefs.minimum_required_padding_size);
        }
        if (strncmp(env_pad_prefs_ptr, "MAX_PAD=", 8) == 0) {
            strsep(&env_pad_prefs_ptr,"=");
            sscanf(env_pad_prefs_ptr, "%" SCNu32, &pad_prefs.maximum_present_padding_size);
        }
    }
    //fprintf(stdout, "Def %" PRIu32 "; Min %" PRIu32 "; Max %" PRIu32 "\n", pad_prefs.default_padding_size, pad_prefs.minimum_required_padding_size, pad_prefs.maximum_present_padding_size);
    return;
}

void GetBasePath(const char *filepath, char* &basepath) {
    //with a myriad of m4a, m4p, mp4, whatever else comes up... it might just be easiest to strip off the end.
    int split_here = 0;
    for (int i=strlen(filepath); i >= 0; i--) {
        const char* this_char=&filepath[i];
        if ( *this_char == '.' ) {
            split_here = i;
            break;
        }
    }
    memcpy(basepath, filepath, (size_t)split_here);

    return;
}

void find_optional_args(char *argv[], int start_optindargs, uint16_t &packed_lang, bool &asUTF16, uint8_t &udta_container, uint8_t &trk_idx, int max_optargs) {
    asUTF16 = false;
    packed_lang = 5575; //und = 0x55C4 = 21956, but QTPlayer doesn't like und //eng =  0x15C7 = 5575

    for (int i= 0; i <= max_optargs-1; i++) {
        if ( argv[start_optindargs + i] && start_optindargs + i <= total_args ) {
            if ( strncmp(argv[start_optindargs + i], "lang=", 5) == 0 ) {
                if (!MatchLanguageCode(argv[start_optindargs +i]+5) ) {
                    packed_lang = PackLanguage("und", 0);
                } else {
                    packed_lang = PackLanguage(argv[start_optindargs +i], 5);
                }

            } else if ( strcmp(argv[start_optindargs + i], "UTF16") == 0 ) {
                asUTF16 = true;
            } else if ( strcmp(argv[optind + i], "movie") == 0 ) {
                udta_container = MOVIE_LEVEL_ATOM;
            } else if ( strncmp(argv[optind + i], "track=", 6) == 0 ) {
                char* track_index_str = argv[optind + i];
                strsep(&track_index_str, "=");
                trk_idx = strtoul(track_index_str, NULL, 10);
                udta_container = SINGLE_TRACK_ATOM;
            } else if ( strcmp(argv[optind + i], "track") == 0 ) {
                udta_container = ALL_TRACKS_ATOM;
            }
            if (*argv[start_optindargs + i] == '-') {
                break; //we've hit another cli argument
            }
        }
    }
    return;
}

void scan_ID3_optargs(char *argv[], int start_optargs, const char* &target_lang, uint16_t &packed_lang, uint8_t &char_encoding, char* meta_container, bool &multistring) {
    packed_lang = 5575; //default ID32 lang is 'eng'
    uint16_t i = 0;

    while (argv[start_optargs + i] != NULL) {
        if ( argv[start_optargs + i] && start_optargs + i <= total_args ) {

            if ( strncmp(argv[start_optargs + i], "lang=", 5) == 0 ) {
                if (!MatchLanguageCode(argv[start_optargs +i]+5) ) {
                    packed_lang = PackLanguage("und", 0);
                    target_lang = "und";
                } else {
                    packed_lang = PackLanguage(argv[start_optargs +i], 5);
                    target_lang = argv[start_optargs + i] + 5;
                }

            } else if ( strcmp(argv[start_optargs + i], "UTF16LE") == 0 ) {
                char_encoding = TE_UTF16LE_WITH_BOM;
            } else if ( strcmp(argv[start_optargs + i], "UTF16BE") == 0 ) {
                char_encoding = TE_UTF16BE_NO_BOM;
            } else if ( strcmp(argv[start_optargs + i], "LATIN1") == 0 ) {
                char_encoding = TE_LATIN1;

            } else if ( strcmp(argv[optind + i], "root") == 0 ) {
                *meta_container = 0-FILE_LEVEL_ATOM;
            } else if ( strncmp(argv[optind + i], "track=", 6) == 0 ) {
                char* track_index_str = argv[optind + i];
                strsep(&track_index_str, "=");
                *meta_container = strtoul(track_index_str, NULL, 10);
            }
        }

        if (*argv[start_optargs + i] == '-') {
            break; //we've hit another cli argument or deleting some frame
        }
        i++;
    }
    return;
}

const char* find_ID3_optarg(char *argv[], int start_optargs, const char* arg_string) {
    const char* ret_val = "";
    uint16_t i = 0;
    uint8_t arg_prefix_len = strlen(arg_string);

    while (argv[start_optargs + i] != NULL) {
        if ( argv[start_optargs + i] && start_optargs + i <= total_args ) {
            if (strcmp(arg_string, "compressed") == 0 && strcmp(argv[start_optargs + i], "compressed") == 0) {
                return "1";
            }
            //if (strcmp(arg_string, "text++") == 0 && strcmp(argv[start_optargs + i], "text++") == 0) {
            //  return "1";
            //}
            if (strncmp(argv[start_optargs + i], arg_string, arg_prefix_len) == 0) {
                ret_val = argv[start_optargs + i] + arg_prefix_len;
                break;
            }
        }
        if (*argv[start_optargs + i] == '-') {
            break; //we've hit another cli argument or deleting some frame
        }
        i++;
    }
    return ret_val;
}

//***********************************************

static void show_short_help(void)
{
    printf("%s\n", shortHelp_text);
    ShowVersionInfo();
    printf("\nReport issues at %s\n", PACKAGE_BUGREPORT);
}

int real_main(int argc, char *argv[])
{
    if (argc == 1) {
        show_short_help();
        exit(0);
    } else if (argc == 2 && (
            (strcmp(argv[1],"-v") == 0) ||
            (strcmp(argv[1],"-version") == 0) ||
            (strcmp(argv[1],"--version") == 0)) ) {
        ShowVersionInfo();
        exit(0);
    } else if (argc == 2) {
        if ((strcmp(argv[1],"-help") == 0) ||
                (strcmp(argv[1],"--help") == 0) ||
                (strcmp(argv[1],"-h") == 0 ) ) {
            show_short_help();
            exit(0);
        } else if ( (strcmp(argv[1],"--longhelp") == 0) || (strcmp(argv[1],"-longhelp") == 0) || (strcmp(argv[1],"-Lh") == 0) ) {
#if defined (_WIN32) && !defined (__CYGWIN__)
            if (UnicodeOutputStatus == WIN32_UTF16) { //convert the helptext to utf16 to preserve © characters
                int help_len = strlen(longHelp_text)+1;
                wchar_t* Lhelp_text = (wchar_t *)malloc(sizeof(wchar_t)*help_len);
                wmemset(Lhelp_text, 0, help_len);
                UTF8ToUTF16LE((unsigned char*)Lhelp_text, 2*help_len, (unsigned char*)longHelp_text, help_len);
                APar_unicode_win32Printout(Lhelp_text, (char *) longHelp_text);
                free(Lhelp_text);
            } else
#endif
            {
                fprintf(stdout, "%s", longHelp_text);
            }
            exit(0);

        } else if ( (strcmp(argv[1],"--3gp-help") == 0) || (strcmp(argv[1],"-3gp-help") == 0) || (strcmp(argv[1],"--3gp-h") == 0) ) {
            fprintf(stdout, "%s\n", _3gpHelp_text); exit(0);

        } else if ( (strcmp(argv[1],"--ISO-help") == 0) || (strcmp(argv[1],"--iso-help") == 0) || (strcmp(argv[1],"-Ih") == 0) ) {
            fprintf(stdout, "%s\n", ISOHelp_text); exit(0);

        } else if ( (strcmp(argv[1],"--file-help") == 0) || (strcmp(argv[1],"-file-help") == 0) || (strcmp(argv[1],"-fh") == 0) ) {
            fprintf(stdout, "%s\n", fileLevelHelp_text); exit(0);

        } else if ( (strcmp(argv[1],"--uuid-help") == 0) || (strcmp(argv[1],"-uuid-help") == 0) || (strcmp(argv[1],"-uh") == 0) ) {
            fprintf(stdout, "%s\n", uuidHelp_text); exit(0);

        } else if ( (strcmp(argv[1],"--reverseDNS-help") == 0) || (strcmp(argv[1],"-rDNS-help") == 0) || (strcmp(argv[1],"-rh") == 0) ) {
            fprintf(stdout, "%s\n", rDNSHelp_text); exit(0);

        } else if ( (strcmp(argv[1],"--ID3-help") == 0) || (strcmp(argv[1],"-ID3-help") == 0) || (strcmp(argv[1],"-ID3h") == 0) ) {
            fprintf(stdout, "%s\n", ID3Help_text); exit(0);

        } else if ( strcmp(argv[1], "--genre-list") == 0 ) {
            ListGenresValues(); exit(0);

        } else if ( strcmp(argv[1], "--stik-list") == 0 ) {
            ListStikValues(); exit(0);

        } else if ( strcmp(argv[1], "--language-list") == 0 ||
                                strcmp(argv[1], "--languages-list") == 0 ||
                                strcmp(argv[1], "--list-language") == 0 ||
                                strcmp(argv[1], "--list-languages") == 0 ||
                                strcmp(argv[1], "-ll") == 0) {
            ListLanguageCodes(); exit(0);

        } else if (strcmp(argv[1], "--ratings-list") == 0) {
            ListMediaRatings(); exit(0);

        } else if (strcmp(argv[1], "--genre-movie-id-list") == 0) {
            ListMovieGenreIDValues(); exit(0);

        } else if (strcmp(argv[1], "--genre-tv-id-list") == 0) {
            ListTVGenreIDValues(); exit(0);

        } else if (strcmp(argv[1], "--ID3frames-list") == 0) {
            ListID3FrameIDstrings(); exit(0);

        } else if (strcmp(argv[1], "--imagetype-list") == 0) {
            List_imagtype_strings(); exit(0);
        }
    }

    if ( argc == 3 && (strcmp(argv[2], "--brands") == 0 || strcmp(argv[2], "-brands") == 0) ) {
        APar_ExtractBrands(argv[1]); exit(0);
    }

    int extr = 99;
    total_args = argc;
    char* ISObasemediafile = argv[1];

    TestFileExistence(ISObasemediafile, true);

    char* padding_options = getenv("AP_PADDING");
    ExtractPaddingPrefs(padding_options);

    //it would probably be better to test output_file if provided & if --overWrite was provided.... probably only of use on Windows - and I'm not on it.
    if (strlen(ISObasemediafile) + 11 > MAXPATHLEN) {
        fprintf(stderr, "%c %s", '\a', "AtomicParsley error: filename/filepath was too long.\n");
        exit(1);
    }

    if ( argc > 3 && strcmp(argv[2], "--DeepScan") == 0) {
        deep_atom_scan = true;
        APar_ScanAtoms(ISObasemediafile, true);
    }

    while (1) {
    static struct option long_options[] = {
        { "help",                             0,  NULL,           OPT_HELP },
        { "test",             optional_argument,  NULL,           OPT_TEST },
        { "textdata",         optional_argument,  NULL,           OPT_ShowTextData },
        { "extractPix",                       0,  NULL,           OPT_ExtractPix },
        { "extractPixToPath", required_argument,  NULL,           OPT_ExtractPixToPath },
        { "artist",           required_argument,  NULL,           Meta_artist },
        { "artDirector",      required_argument,  NULL,           Meta_artDirector },
        { "arranger",         required_argument,  NULL,           Meta_arranger },
        { "author",           required_argument,  NULL,           Meta_author },
        { "conductor",        required_argument,  NULL,           Meta_conductor },
        { "director",         required_argument,  NULL,           Meta_director },
        { "originalArtist",   required_argument,  NULL,           Meta_originalArtist },
        { "producer",         required_argument,  NULL,           Meta_producer },
//        { "performer",        required_argument,  NULL,           Meta_performer },
        { "soundEngineer",    required_argument,  NULL,           Meta_soundEngineer },
        { "soloist",          required_argument,  NULL,           Meta_soloist },
        { "executiveProducer",required_argument,  NULL,           Meta_executiveProducer },
        { "title",            required_argument,  NULL,           Meta_songtitle },
        { "subtitle",         required_argument,  NULL,           Meta_subtitle },
        { "album",            required_argument,  NULL,           Meta_album },
        { "genre",            required_argument,  NULL,           Meta_genre },
        { "tracknum",         required_argument,  NULL,           Meta_tracknum },
        { "disknum",          required_argument,  NULL,           Meta_disknum },
        { "comment",          required_argument,  NULL,           Meta_comment },
        { "year",             required_argument,  NULL,           Meta_year },
        { "lyrics",           required_argument,  NULL,           Meta_lyrics },
        { "lyricsFile",       required_argument,  NULL,           Meta_lyrics_file },
        { "composer",         required_argument,  NULL,           Meta_composer },
        { "copyright",        required_argument,  NULL,           Meta_copyright },
        { "grouping",         required_argument,  NULL,           Meta_grouping },
        { "albumArtist",      required_argument,  NULL,           Meta_album_artist },
        { "compilation",      required_argument,  NULL,           Meta_compilation },
        { "hdvideo",          required_argument,  NULL,           Meta_hdvideo },
        { "advisory",         required_argument,  NULL,           Meta_advisory },
        { "bpm",              required_argument,  NULL,           Meta_BPM },
        { "artwork",          required_argument,  NULL,           Meta_artwork },
        { "stik",             required_argument,  NULL,           Meta_stik },
        { "description",      required_argument,  NULL,           Meta_description },
        { "longdesc",         required_argument,  NULL,           Meta_longdescription },
        { "storedesc",        required_argument,  NULL,           Meta_storedescription },
        { "Rating",           required_argument,  NULL,           Meta_Rating },
        { "TVNetwork",        required_argument,  NULL,           Meta_TV_Network },
        { "TVShowName",       required_argument,  NULL,           Meta_TV_ShowName },
        { "TVEpisode",        required_argument,  NULL,           Meta_TV_Episode },
        { "TVEpisodeNum",     required_argument,  NULL,           Meta_TV_EpisodeNumber },
        { "TVSeasonNum",      required_argument,  NULL,           Meta_TV_SeasonNumber },
        { "podcastFlag",      required_argument,  NULL,           Meta_podcastFlag },
        { "keyword",          required_argument,  NULL,           Meta_keyword },
        { "category",         required_argument,  NULL,           Meta_category },
        { "podcastURL",       required_argument,  NULL,           Meta_podcast_URL },
        { "podcastGUID",      required_argument,  NULL,           Meta_podcast_GUID },
        { "purchaseDate",     required_argument,  NULL,           Meta_PurchaseDate },
        { "encodingTool",     required_argument,  NULL,           Meta_EncodingTool },
        { "encodedBy",        required_argument,  NULL,           Meta_EncodedBy },
        { "apID",             required_argument,  NULL,           Meta_apID },
        { "cnID",             required_argument,  NULL,           Meta_cnID },
        { "geID",             required_argument,  NULL,           Meta_geID },
        { "xID",              required_argument,  NULL,           Meta_xID },
        { "gapless",          required_argument,  NULL,           Meta_PlayGapless },
        { "sortOrder",        required_argument,  NULL,           Meta_SortOrder } ,

        { "rDNSatom",         required_argument,  NULL,           Meta_ReverseDNS_Form },
        { "contentRating",    required_argument,  NULL,           Meta_rDNS_rating },

        { "tagtime",          optional_argument,  NULL,           Meta_StandardDate },
        { "information",      required_argument,  NULL,           Meta_Information },
        { "url",              required_argument,  NULL,           Meta_URL },
        { "meta-uuid",        required_argument,  NULL,           Meta_uuid },
        { "extract-uuids",    optional_argument,  NULL,           Opt_Extract_all_uuids },
        { "extract1uuid",     required_argument,  NULL,           Opt_Extract_a_uuid },
        { "iPod-uuid",        required_argument,  NULL,           Opt_Ipod_AVC_uuid },

        { "freefree",         optional_argument,  NULL,           Opt_FreeFree },
        { "metaEnema",        0,                  NULL,           Metadata_Purge },
        { "manualAtomRemove", required_argument,  NULL,           Manual_atom_removal },
        { "udtaEnema",        0,                  NULL,           UserData_Purge },
        { "foobar2000Enema",  0,                  NULL,           foobar_purge },
        { "metaDump",         0,                  NULL,           Meta_dump },
        { "output",           required_argument,  NULL,           OPT_OutputFile },
        { "preventOptimizing",0,                  NULL,           OPT_NoOptimize },
        { "overWrite",        0,                  NULL,           OPT_OverWrite },
#if defined (_WIN32)
        { "preserveTime",        0,               NULL,           OPT_PreserveTimeStamps },
#endif
        { "ISO-copyright",    required_argument,  NULL,           ISO_Copyright },

        { "3gp-title",        required_argument,  NULL,           _3GP_Title },
        { "3gp-author",       required_argument,  NULL,           _3GP_Author },
        { "3gp-performer",    required_argument,  NULL,           _3GP_Performer },
        { "3gp-genre",        required_argument,  NULL,           _3GP_Genre },
        { "3gp-description",  required_argument,  NULL,           _3GP_Description },
        { "3gp-copyright",    required_argument,  NULL,           _3GP_Copyright },
        { "3gp-album",        required_argument,  NULL,           _3GP_Album },
        { "3gp-year",         required_argument,  NULL,           _3GP_Year },

        { "3gp-rating",       required_argument,  NULL,           _3GP_Rating },
        { "3gp-classification",  required_argument,  NULL,          _3GP_Classification },
        { "3gp-keyword",      required_argument,  NULL,           _3GP_Keyword },
        { "3gp-location",     required_argument,  NULL,           _3GP_Location },

        { "ID3Tag",           required_argument,  NULL,           Meta_ID3v2Tag },

        { "DeepScan",         0,                  &extr,          1 },

        { 0, 0, 0, 0 }
    };

    int c = -1;
    int option_index = 0;

    c = getopt_long(argc, argv, "hTtEe:a:b:c:d:f:g:i:k:l:n:o:p:q::u:w:x:y:z:A:B:C:D:F:G:H:I:J:K:L:MN:QR:S:U:WXV:ZP", long_options, &option_index);

    if (c == -1) {
        if (argc < 3 && argc > 2) {
            APar_ScanAtoms(ISObasemediafile, true);
            APar_PrintAtomicTree();
        }
        break;
    }

    signal(SIGTERM, kill_signal);
    signal(SIGINT,  kill_signal);

    switch(c) {
        // "optind" represents the count of arguments up to and including its optional flag:

        case '?': return 1;

        case OPT_HELP: {
            fprintf (stdout,"%s", longHelp_text); return 0;
        }

        case OPT_TEST: {
            deep_atom_scan = true;
            APar_ScanAtoms(ISObasemediafile, true);
            APar_PrintAtomicTree();
            if (argv[optind]) {
                if (strcmp(argv[optind], "+dates") == 0) {
                    APar_ExtractDetails( APar_OpenISOBaseMediaFile(ISObasemediafile, true), SHOW_TRACK_INFO + SHOW_DATE_INFO );
                } else {
                    APar_ExtractDetails( APar_OpenISOBaseMediaFile(ISObasemediafile, true), SHOW_TRACK_INFO);
                }
            }
            APar_OpenISOBaseMediaFile(ISObasemediafile, false);
            break;
        }

        case OPT_ShowTextData: {
            if (argv[optind]) { //for utilities that write iTunes-style metadata into 3gp branded files
                APar_ExtractBrands(ISObasemediafile);
                deep_atom_scan=true;
                APar_ScanAtoms(ISObasemediafile);

                APar_OpenISOBaseMediaFile(ISObasemediafile, true);

                if (strcmp(argv[optind], "+") == 0) {
                    APar_Print_iTunesData(ISObasemediafile, NULL, PRINT_FREE_SPACE + PRINT_PADDING_SPACE + PRINT_USER_DATA_SPACE + PRINT_MEDIA_SPACE, PRINT_DATA );
                } else {
                    fprintf(stdout, "---------------------------\n");
                    APar_Print_ISO_UserData_per_track();

                    AtomicInfo* iTuneslistAtom = APar_FindAtom("moov.udta.meta.ilst", false, SIMPLE_ATOM, 0);
                    if (iTuneslistAtom != NULL) {
                        fprintf(stdout, "---------------------------\n  iTunes-style metadata tags:\n");
                        APar_Print_iTunesData(ISObasemediafile, NULL, PRINT_FREE_SPACE + PRINT_PADDING_SPACE + PRINT_USER_DATA_SPACE + PRINT_MEDIA_SPACE, PRINT_DATA, iTuneslistAtom );
                    }
                    fprintf(stdout, "---------------------------\n");
                }

            } else {
                deep_atom_scan=true;
                APar_ScanAtoms(ISObasemediafile);
                APar_OpenISOBaseMediaFile(ISObasemediafile, true);

                if (metadata_style >= THIRD_GEN_PARTNER) {
                    APar_PrintUserDataAssests();
                } else if (metadata_style == ITUNES_STYLE) {
                    APar_Print_iTunesData(ISObasemediafile, NULL, 0, PRINT_DATA); //false, don't try to extractPix
                    APar_Print_APuuid_atoms(ISObasemediafile, NULL, PRINT_DATA);
                }
            }
            APar_OpenISOBaseMediaFile(ISObasemediafile, false);
            break;
        }

        case OPT_ExtractPix: {
            char* base_path=(char*)malloc(sizeof(char)*MAXPATHLEN+1);
            memset(base_path, 0, MAXPATHLEN +1);

            GetBasePath( ISObasemediafile, base_path );
            APar_ScanAtoms(ISObasemediafile);
            APar_OpenISOBaseMediaFile(ISObasemediafile, true);
            APar_Print_iTunesData(ISObasemediafile, base_path, 0, EXTRACT_ARTWORK); //exportPix to stripped ISObasemediafile path
            APar_OpenISOBaseMediaFile(ISObasemediafile, false);

            free(base_path);
            base_path = NULL;
            break;
        }

        case OPT_ExtractPixToPath: {
            APar_ScanAtoms(ISObasemediafile);
            APar_OpenISOBaseMediaFile(ISObasemediafile, true);
            APar_Print_iTunesData(ISObasemediafile, optarg, 0, EXTRACT_ARTWORK); //exportPix to a different path
            APar_OpenISOBaseMediaFile(ISObasemediafile, false);
            break;
        }

        case Meta_artist : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "artist") ) {
                char major_brand[4];
                UInt32_TO_String4(brand, &*major_brand);
                APar_assert(false, 4, &*major_brand);
                break;
            }
            AtomicInfo* artistData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©ART.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(artistData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_artDirector : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "artDirector") ) {
                char major_brand[4];
                UInt32_TO_String4(brand, &*major_brand);
                APar_assert(false, 4, &*major_brand);
                break;
            }
            AtomicInfo* artistData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©ard.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(artistData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_arranger : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "arranger") ) {
                char major_brand[4];
                UInt32_TO_String4(brand, &*major_brand);
                APar_assert(false, 4, &*major_brand);
                break;
            }
            AtomicInfo* artistData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©arg.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(artistData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_author : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "author") ) {
                char major_brand[4];
                UInt32_TO_String4(brand, &*major_brand);
                APar_assert(false, 4, &*major_brand);
                break;
            }
            AtomicInfo* artistData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©aut.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(artistData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_conductor : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "conductor") ) {
                char major_brand[4];
                UInt32_TO_String4(brand, &*major_brand);
                APar_assert(false, 4, &*major_brand);
                break;
            }
            AtomicInfo* artistData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©con.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(artistData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_director : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "director") ) {
                char major_brand[4];
                UInt32_TO_String4(brand, &*major_brand);
                APar_assert(false, 4, &*major_brand);
                break;
            }
            AtomicInfo* artistData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©dir.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(artistData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_originalArtist : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "originalArtist") ) {
                char major_brand[4];
                UInt32_TO_String4(brand, &*major_brand);
                APar_assert(false, 4, &*major_brand);
                break;
            }
            AtomicInfo* artistData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©ope.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(artistData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_producer : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "producer") ) {
                char major_brand[4];
                UInt32_TO_String4(brand, &*major_brand);
                APar_assert(false, 4, &*major_brand);
                break;
            }
            AtomicInfo* artistData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©prd.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(artistData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

/*
        case Meta_performer : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "performer") ) {
                char major_brand[4];
                UInt32_TO_String4(brand, &*major_brand);
                APar_assert(false, 4, &*major_brand);
                break;
            }
            AtomicInfo* artistData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©prf.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(artistData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }
*/

        case Meta_soundEngineer : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "soundEngineer") ) {
                char major_brand[4];
                UInt32_TO_String4(brand, &*major_brand);
                APar_assert(false, 4, &*major_brand);
                break;
            }
            AtomicInfo* artistData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©sne.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(artistData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_soloist : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "soloist") ) {
                char major_brand[4];
                UInt32_TO_String4(brand, &*major_brand);
                APar_assert(false, 4, &*major_brand);
                break;
            }
            AtomicInfo* artistData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©sol.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(artistData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_executiveProducer : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "executiveProducer") ) {
                char major_brand[4];
                UInt32_TO_String4(brand, &*major_brand);
                APar_assert(false, 4, &*major_brand);
                break;
            }
            AtomicInfo* artistData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©xpd.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(artistData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_songtitle : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "title") ) {
                break;
            }

            AtomicInfo* titleData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©nam.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(titleData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_subtitle : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "subtitle") ) {
                break;
            }

            AtomicInfo* titleData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©st3.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(titleData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_album : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "album") ) {
                break;
            }

            AtomicInfo* albumData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©alb.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(albumData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_genre : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "genre") ) {
                break;
            }

            APar_MetaData_atomGenre_Set(optarg);
            break;
        }

        case Meta_tracknum : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "track number") ) {
                break;
            }

            uint16_t pos_in_total = 0;
            uint16_t the_total = 0;
            if (strrchr(optarg, '/') != NULL) {

                char* duplicate_info = optarg;
                char* item_stat = strsep(&duplicate_info,"/");
                sscanf(item_stat, "%" SCNu16, &pos_in_total);
                item_stat = strsep(&duplicate_info,"/");
                sscanf(item_stat, "%" SCNu16, &the_total);
            } else {
                sscanf(optarg, "%" SCNu16, &pos_in_total);
            }

            AtomicInfo* tracknumData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.trkn.data", optarg, AtomFlags_Data_Binary);
            //tracknum: [0, 0, 0, 0,   0, 0, 0, pos_in_total, 0, the_total, 0, 0]; BUT that first uint32_t is already accounted for in APar_MetaData_atom_Init
            APar_Unified_atom_Put(tracknumData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, 0, 16);
            APar_Unified_atom_Put(tracknumData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, pos_in_total, 16);
            APar_Unified_atom_Put(tracknumData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, the_total, 16);
            APar_Unified_atom_Put(tracknumData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, 0, 16);
            break;
        }

        case Meta_disknum : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "disc number") ) {
                break;
            }

            uint16_t pos_in_total = 0;
            uint16_t the_total = 0;
            if (strrchr(optarg, '/') != NULL) {

                char* duplicate_info = optarg;
                char* item_stat = strsep(&duplicate_info,"/");
                sscanf(item_stat, "%" SCNu16, &pos_in_total);
                item_stat = strsep(&duplicate_info,"/");
                sscanf(item_stat, "%" SCNu16, &the_total);

            } else {
                sscanf(optarg, "%" SCNu16, &pos_in_total);
            }

            AtomicInfo* disknumData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.disk.data", optarg, AtomFlags_Data_Binary);
            //disknum: [0, 0, 0, 0,   0, 0, 0, pos_in_total, 0, the_total]; BUT that first uint32_t is already accounted for in APar_MetaData_atom_Init
            APar_Unified_atom_Put(disknumData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, 0, 16);
            APar_Unified_atom_Put(disknumData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, pos_in_total, 16);
            APar_Unified_atom_Put(disknumData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, the_total, 16);
            break;
        }

        case Meta_comment : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "comment") ) {
                break;
            }

            AtomicInfo* commentData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©cmt.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(commentData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_year : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "year") ) {
                break;
            }

            AtomicInfo* yearData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©day.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(yearData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_lyrics : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "lyrics") ) {
                break;
            }

            AtomicInfo* lyricsData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©lyr.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(lyricsData_atom, optarg, UTF8_iTunesStyle_Unlimited, 0, 0);
            break;
        }

        case Meta_lyrics_file : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "lyrics") ) {
                break;
            }

            APar_MetaData_atomLyrics_Set(optarg);
            break;
        }

        case Meta_composer : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "composer") ) {
                break;
            }

            AtomicInfo* composerData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©wrt.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(composerData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_copyright : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "copyright") ) {
                break;
            }

            AtomicInfo* copyrightData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.cprt.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(copyrightData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_grouping : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "grouping") ) {
                break;
            }

            AtomicInfo* groupingData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©grp.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(groupingData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_compilation : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "compilation") ) {
                break;
            }

            if (strcmp(optarg, "false") == 0 || strlen(optarg) == 0) {
                APar_RemoveAtom("moov.udta.meta.ilst.cpil.data", VERSIONED_ATOM, 0);
            } else {
                //compilation: [0, 0, 0, 0,   boolean_value]; BUT that first uint32_t is already accounted for in APar_MetaData_atom_Init
                AtomicInfo* compilationData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.cpil.data", optarg, AtomFlags_Data_UInt);
                APar_Unified_atom_Put(compilationData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, 1, 8); //a hard coded uint8_t of: 1 is compilation
            }
            break;
        }

        case Meta_hdvideo : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "hdvideo") ) {
                break;
            }

            if (strcmp(optarg, "false") == 0 || strlen(optarg) == 0) {
                APar_RemoveAtom("moov.udta.meta.ilst.hdvd.data", VERSIONED_ATOM, 0);
            } else {
                //compilation: [0, 0, 0, 0,   boolean_value]; BUT that first uint32_t is already accounted for in APar_MetaData_atom_Init
                AtomicInfo* hdvideoData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.hdvd.data", optarg, AtomFlags_Data_UInt);
                APar_Unified_atom_Put(hdvideoData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, 1, 8); //a hard coded uint8_t of: 1 is compilation
            }
            break;
        }

        case Meta_BPM : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "BPM") ) {
                break;
            }

            if (strcmp(optarg, "0") == 0 || strlen(optarg) == 0) {
                APar_RemoveAtom("moov.udta.meta.ilst.tmpo.data", VERSIONED_ATOM, 0);
            } else {
                uint16_t bpm_value = 0;
                sscanf(optarg, "%" SCNu16, &bpm_value );
                //bpm is [0, 0, 0, 0,   0, bpm_value]; BUT that first uint32_t is already accounted for in APar_MetaData_atom_Init
                AtomicInfo* bpmData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.tmpo.data", optarg, AtomFlags_Data_UInt);
                APar_Unified_atom_Put(bpmData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, bpm_value, 16);
            }
            break;
        }

        case Meta_advisory : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "content advisory") ) {
                break;
            }

            if (strcmp(optarg, "remove") == 0 || strlen(optarg) == 0) {
                APar_RemoveAtom("moov.udta.meta.ilst.rtng.data", VERSIONED_ATOM, 0);
            } else {
                uint8_t rating_value = 0;
                if (strcmp(optarg, "clean") == 0) {
                    rating_value = 2; //only \02 is clean
                } else if (strcmp(optarg, "explicit") == 0) {
                    rating_value = 4; //most non \00, \02 numbers are allowed
                }
                //rating is [0, 0, 0, 0,   rating_value]; BUT that first uint32_t is already accounted for in APar_MetaData_atom_Init
                AtomicInfo* advisoryData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.rtng.data", optarg, AtomFlags_Data_UInt);
                APar_Unified_atom_Put(advisoryData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, rating_value, 8);
            }
            break;
        }

        case Meta_artwork : { //handled differently: there can be multiple "moov.udta.meta.ilst.covr.data" atoms
            char* env_PicOptions = getenv("PIC_OPTIONS");
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "coverart") ) {
                break;
            }

            APar_MetaData_atomArtwork_Set(optarg, env_PicOptions);
            break;
        }

        case Meta_stik : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "'stik'") ) {
                break;
            }

            if (strcmp(optarg, "remove") == 0 || strlen(optarg) == 0) {
                APar_RemoveAtom("moov.udta.meta.ilst.stik.data", VERSIONED_ATOM, 0);
            } else {
                uint8_t stik_value = 0;

                if (strncmp(optarg, "value=", 6) == 0) {
                    char* stik_val_str_ptr = optarg;
                    strsep(&stik_val_str_ptr,"=");
                    stik_value = strtoul(stik_val_str_ptr, NULL, 10);
                } else {
                    stiks* return_stik = MatchStikString(optarg);
                    if (return_stik != NULL) {
                        stik_value = return_stik->stik_number;
                        if (strcmp(optarg, "Audiobook") == 0) {
                            forced_suffix_type = FORCE_M4B_TYPE;
                        }
                    }
                }
                //stik is [0, 0, 0, 0,   stik_value]; BUT that first uint32_t is already accounted for in APar_MetaData_atom_Init
                AtomicInfo* stikData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.stik.data", optarg, AtomFlags_Data_UInt);
                APar_Unified_atom_Put(stikData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, stik_value, 8);
            }
            break;
        }

        case Meta_EncodingTool : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "encoding tool") ) {
                break;
            }

            AtomicInfo* encodingtoolData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©too.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(encodingtoolData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_EncodedBy : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "encoded by") ) {
                break;
            }

            AtomicInfo* encodedbyData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.©enc.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(encodedbyData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_apID : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "Account Name") ) {
                break;
            }

            AtomicInfo* apIDData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.apID.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(apIDData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_description : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "description") ) {
                break;
            }

            AtomicInfo* descriptionData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.desc.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(descriptionData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_longdescription : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "longdesc") ) {
                break;
            }

            AtomicInfo* descriptionData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.ldes.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(descriptionData_atom, optarg, UTF8_iTunesStyle_Unlimited, 0, 0);
            break;
        }

        case Meta_storedescription : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "storedesc") ) {
                break;
            }

            AtomicInfo* descriptionData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.sdes.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(descriptionData_atom, optarg, UTF8_iTunesStyle_Unlimited, 0, 0);
            break;
        }

        case Meta_Rating : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "Rating") ) {
                break;
            }

            AtomicInfo* ratingData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.rate.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(ratingData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_TV_Network : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "TV Network") ) {
                break;
            }

            AtomicInfo* tvnetworkData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.tvnn.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(tvnetworkData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_TV_ShowName : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "TV Show name") ) {
                break;
            }

            AtomicInfo* tvshownameData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.tvsh.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(tvshownameData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_TV_Episode : { //if the show "ABC Lost 209", its "209"
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "TV Episode string") ) {
                break;
            }

            AtomicInfo* tvepisodeData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.tven.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(tvepisodeData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_TV_SeasonNumber : { //if the show "ABC Lost 209", its 2; integer 2 not char "2"
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "TV Season") ) {
                break;
            }

            uint16_t data_value = 0;
            sscanf(optarg, "%" SCNu16, &data_value );

            AtomicInfo* tvseasonData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.tvsn.data", optarg, AtomFlags_Data_UInt);
            //season is [0, 0, 0, 0,   0, 0, 0, data_value]; BUT that first uint32_t is already accounted for in APar_MetaData_atom_Init
            APar_Unified_atom_Put(tvseasonData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, 0, 16);
            APar_Unified_atom_Put(tvseasonData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, data_value, 16);
            break;
        }

        case Meta_TV_EpisodeNumber : { //if the show "ABC Lost 209", its 9; integer 9 (0x09) not char "9"(0x39)
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "TV Episode number") ) {
                break;
            }

            uint16_t data_value = 0;
            sscanf(optarg, "%" SCNu16, &data_value );

            AtomicInfo* tvepisodenumData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.tves.data", optarg, AtomFlags_Data_UInt);
            //episodenumber is [0, 0, 0, 0,   0, 0, 0, data_value]; BUT that first uint32_t is already accounted for in APar_MetaData_atom_Init
            APar_Unified_atom_Put(tvepisodenumData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, 0, 16);
            APar_Unified_atom_Put(tvepisodenumData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, data_value, 16);
            break;
        }

        case Meta_cnID : { // the iTunes Catalog ID
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "iTunes Catalog ID") ) {
                break;
            }

            uint32_t data_value = 0;
            sscanf(optarg, "%" SCNu32, &data_value );

            AtomicInfo* cnIDData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.cnID.data", optarg, AtomFlags_Data_UInt);
            APar_Unified_atom_Put(cnIDData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, data_value, 32);
            break;
        }

        case Meta_geID : { // the iTunes Genre ID
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "iTunes Genre ID") ) {
                break;
            }

            uint32_t data_value = 0;
            sscanf(optarg, "%" SCNu32, &data_value );

            AtomicInfo* geIDData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.geID.data", optarg, AtomFlags_Data_UInt);
            APar_Unified_atom_Put(geIDData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, data_value, 32);
            break;
        }

        case Meta_xID : { // the vendor-supplied iTunes xID
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "iTunes xID") ) {
                break;
            }

            AtomicInfo* xIDData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.xid .data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(xIDData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_album_artist : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "album artist") ) {
                break;
            }

            AtomicInfo* albumartistData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.aART.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(albumartistData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_podcastFlag : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "podcast flag") ) {
                break;
            }

            if (strcmp(optarg, "false") == 0) {
                APar_RemoveAtom("moov.udta.meta.ilst.pcst.data", VERSIONED_ATOM, 0);
            } else {
                //podcastflag: [0, 0, 0, 0,   boolean_value]; BUT that first uint32_t is already accounted for in APar_MetaData_atom_Init
                AtomicInfo* podcastFlagData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.pcst.data", optarg, AtomFlags_Data_UInt);
                APar_Unified_atom_Put(podcastFlagData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, 1, 8); //a hard coded uint8_t of: 1 denotes podcast flag
            }

            break;
        }

        case Meta_keyword : {    //TODO to the end of iTunes-style metadata & uuid atoms
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "keyword") ) {
                break;
            }

            AtomicInfo* keywordData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.keyw.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(keywordData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_category : { // see http://www.apple.com/itunes/podcasts/techspecs.html for available categories
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "category") ) {
                break;
            }

            AtomicInfo* categoryData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.catg.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(categoryData_atom, optarg, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            break;
        }

        case Meta_podcast_URL : { // usually a read-only value, but useful for getting videos into the 'podcast' menu
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "podcast URL") ) {
                break;
            }

            AtomicInfo* podcasturlData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.purl.data", optarg, AtomFlags_Data_Binary);
            APar_Unified_atom_Put(podcasturlData_atom, optarg, UTF8_iTunesStyle_Binary, 0, 0);
            break;
        }

        case Meta_podcast_GUID : { // Global Unique IDentifier; it is *highly* doubtful that this would be useful...
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "podcast GUID") ) {
                break;
            }

            AtomicInfo* globalidData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.egid.data", optarg, AtomFlags_Data_Binary);
            APar_Unified_atom_Put(globalidData_atom, optarg, UTF8_iTunesStyle_Binary, 0, 0);
            break;
        }

        case Meta_PurchaseDate : { // might be useful to *remove* this, but adding it... although it could function like id3v2 tdtg...
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "purchase date") ) {
                break;
            }
            char* purd_time;
            bool free_memory = false;
            if (optarg != NULL) {
                if (strcmp(optarg, "timestamp") == 0) {
                    purd_time = (char *)malloc(sizeof(char)*255);
                    free_memory = true;
                    APar_StandardTime(purd_time);
                } else {
                    purd_time = optarg;
                }
            } else {
                purd_time = optarg;
            }

            AtomicInfo* globalIDData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.purd.data", optarg, AtomFlags_Data_Text);
            APar_Unified_atom_Put(globalIDData_atom, purd_time, UTF8_iTunesStyle_256glyphLimited, 0, 0);
            if (free_memory) {
                free(purd_time);
                purd_time = NULL;
            }
            break;
        }

        case Meta_PlayGapless : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "gapless playback") ) {
                break;
            }

            if (strcmp(optarg, "false") == 0 || strlen(optarg) == 0) {
                APar_RemoveAtom("moov.udta.meta.ilst.pgap.data", VERSIONED_ATOM, 0);
            } else {
                //gapless playback: [0, 0, 0, 0,   boolean_value]; BUT that first uint32_t is already accounted for in APar_MetaData_atom_Init
                AtomicInfo* gaplessData_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.pgap.data", optarg, AtomFlags_Data_UInt);
                APar_Unified_atom_Put(gaplessData_atom, NULL, UTF8_iTunesStyle_256glyphLimited, 1, 8);
            }
            break;
        }

        case Meta_SortOrder : {
            AtomicInfo* sortOrder_atom = NULL;
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "sort order tags") ) {
                break;
            }

            if (argv[optind] == NULL) {
                fprintf(stdout, "AP warning, skipping setting the sort order %s tag\n", optarg);
                break;
            }

            if ( strcmp(optarg, "name") == 0 ) {
                sortOrder_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.sonm.data", argv[optind], AtomFlags_Data_Text);
            } else if ( strcmp(optarg, "artist") == 0 ) {
                sortOrder_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.soar.data", argv[optind], AtomFlags_Data_Text);
            } else if ( strcmp(optarg, "albumartist") == 0 ) {
                sortOrder_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.soaa.data", argv[optind], AtomFlags_Data_Text);
            } else if ( strcmp(optarg, "album") == 0 ) {
                sortOrder_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.soal.data", argv[optind], AtomFlags_Data_Text);
            } else if ( strcmp(optarg, "composer") == 0 ) {
                sortOrder_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.soco.data", argv[optind], AtomFlags_Data_Text);
            } else if ( strcmp(optarg, "show") == 0 ) {
                sortOrder_atom = APar_MetaData_atom_Init("moov.udta.meta.ilst.sosn.data", argv[optind], AtomFlags_Data_Text);
            }
            APar_Unified_atom_Put(sortOrder_atom, argv[optind], UTF8_iTunesStyle_256glyphLimited, 0, 0);

            break;
        }

        //uuid atoms

        case Meta_StandardDate : {
            APar_ScanAtoms(ISObasemediafile);
            char* formed_time = (char *)malloc(sizeof(char)*110);
            if (argv[optind]) {
                if (strlen(argv[optind]) > 0) {
                    APar_StandardTime(formed_time);
                }
            }

            AtomicInfo* tdtgUUID = APar_uuid_atom_Init("moov.udta.meta.uuid=%s", "tdtg", AtomFlags_Data_Text, formed_time, false);
            APar_Unified_atom_Put(tdtgUUID, formed_time, UTF8_iTunesStyle_Unlimited, 0, 0);
            free(formed_time);
            break;
        }

        case Meta_URL : {
            APar_ScanAtoms(ISObasemediafile);
            AtomicInfo* urlUUID = APar_uuid_atom_Init("moov.udta.meta.uuid=%s", "©url", AtomFlags_Data_Text, optarg, false);
            APar_Unified_atom_Put(urlUUID, optarg, UTF8_iTunesStyle_Unlimited, 0, 0);
            break;
        }

        case Meta_Information : {
            APar_ScanAtoms(ISObasemediafile);
            AtomicInfo* infoUUID = APar_uuid_atom_Init("moov.udta.meta.uuid=%s", "©inf", AtomFlags_Data_Text, optarg, false);
            APar_Unified_atom_Put(infoUUID, optarg, UTF8_iTunesStyle_Unlimited, 0, 0);
            break;
        }

        case Meta_uuid : {
            APar_ScanAtoms(ISObasemediafile);
            uint32_t uuid_dataType = 0;
            uint32_t desc_len = 0;
            uint8_t mime_len = 0;
            char* uuid_file_path = NULL;
            char* uuid_file_description = NULL;
            char* uuid_file_extn = NULL;
            char* uuid_file_mimetype = NULL;
//          char* uuid_file_filename = NULL;

            // a uuid in AP is a version 5 uuid created by getting a sha1 hash
            // of a string (the atom name) in a namespace ('AP.sf.net'). This
            // is guaranteed to be reproducible, so later it can be verified
            // that this uuid (which could come from anywhere), is in fact made
            // by AtomicParsley. This is achieved by storing the atom name
            // string right after the uuid, and is read back later and a new
            // uuid is created to see if it matches the discovered uuid. If
            // they match, it will print out or extract to a file; if not, only
            // its name will be displayed in the tree.
            //
            // --meta-uuid "©foo" 1 'http://www.url.org'  --meta-uuid "pdf1"
            // file /some/path/pic.pdf description="My Booty, Your Booty,
            // Djbouti"

            if ( strcmp(argv[optind], "text") == 0  || strcmp(argv[optind], "1") == 0 ) uuid_dataType = AtomFlags_Data_Text;
            if ( strcmp(argv[optind], "file") == 0 ) {
                uuid_dataType = AtomFlags_Data_uuid_binary;
                if (optind+1 < argc) {
                    if (true) { //TODO: test the file to see if it exists
                        uuid_file_path = argv[optind + 1];
                        //get the file extension/suffix of the file to embed
                        uuid_file_extn = strrchr(uuid_file_path, '.'); //'.' inclusive; say goodbye to AP-0.8.8.tar.bz2

//#ifdef _WIN32
//#define path_delim '\\'
//#else
//#define path_delim '/'
//#endif
//                      uuid_file_filename = strrchr(uuid_file_path, path_delim)+1; //includes whatever extensions
                    }
                    if (uuid_file_extn == NULL) {
                        fprintf(stdout, "AP warning: embedding a file onto a uuid atom requires a file extension. Skipping.\n");
                        continue;
                    }
                    //copy a pointer to description
                    int more_optional_args = 2;
                    while (optind + more_optional_args < argc) {
                        if ( strncmp(argv[optind + more_optional_args], "description=", 12) == 0 && argv[optind + more_optional_args][12]) {
                            uuid_file_description = argv[optind + more_optional_args] + 12;
                            desc_len = strlen(uuid_file_description)+1; //+1 for the trailing 1 byte NULL terminator
                        }
                        if ( strncmp(argv[optind + more_optional_args], "mime-type=", 10) == 0 && argv[optind + more_optional_args][10]) {
                            uuid_file_mimetype = argv[optind + more_optional_args] + 10;
                            mime_len = strlen(uuid_file_mimetype)+1; //+1 for the trailing 1 byte NULL terminator
                        }
                        if (strncmp(argv[optind+more_optional_args], "--", 2) == 0) {
                            break; //we've hit another cli argument
                        }
                        more_optional_args++;
                    }
                }
            }

            AtomicInfo* genericUUID = APar_uuid_atom_Init("moov.udta.meta.uuid=%s", optarg, uuid_dataType, argv[optind +1], true);

            if (uuid_dataType == AtomFlags_Data_uuid_binary && genericUUID != NULL) {
                TestFileExistence(uuid_file_path, true);

//format for a uuid atom set by AP:
//4 bytes     - atom length as uin32_t
//4 bytes     - atom name as iso 8859-1 atom name as a 4byte string set to 'uuid'
//16 bytes    - the uuid; here a version 5 sha1-based hash derived from a name in a namespace of 'AtomicParsley.sf.net'
//4 bytes     - the name of the desired atom to create a uuid for (this "name" of the uuid is the only cli accessible means of crafting the uuid)
//4 bytes     - atom version & flags (currently 1 for 'text' or '88' for binary attachment)
//4 bytes     - NULL space
/////////////text or 1 for version/flags
        //X bytes - utf8 string, no null termination
/////////////binary attachment or 88 for version/flags
        //4 bytes - length of utf8 string describing the attachment
        //X bytes - utf8 string describing the attachment, null terminated
        //1 byte  - length of the file suffix (including the period) of the originating file
        //X bytes - utf8 string of the file suffix, null terminated
        //1 byte  - length of the MIME-type
        //X bytes - utf8 string holding the MIME-type, null terminated
        //4 bytes - length of the attached binary data/file length
        //X bytes - binary data

                uint32_t extn_len = strlen(uuid_file_extn)+1; //+1 for the trailing 1 byte NULL terminator
                uint64_t file_len = findFileSize(uuid_file_path);

                APar_MetaData_atom_QuickInit(genericUUID->AtomicNumber, uuid_dataType, 20, extn_len + desc_len + file_len + 100);
                genericUUID->AtomicClassification = EXTENDED_ATOM; //it gets reset in QuickInit Above; force its proper setting

                if (uuid_file_description == NULL || desc_len == 0) {
                    APar_Unified_atom_Put(genericUUID, "[none]", UTF8_3GP_Style, 7, 32); //sets 4bytes desc_len, then 7bytes description (forced to "[none]"=6 + 1 byte NULL =7)
                } else {
                    APar_Unified_atom_Put(genericUUID, uuid_file_description, UTF8_3GP_Style, desc_len, 32); //sets 4bytes desc_len, then Xbytes description (in that order)
                }

                APar_Unified_atom_Put(genericUUID, uuid_file_extn, UTF8_3GP_Style, extn_len, 8); //sets 1bytes desc_len, then Xbytes file extension string (in that order)

                if (uuid_file_mimetype == NULL || mime_len == 0) {
                    APar_Unified_atom_Put(genericUUID, "none", UTF8_3GP_Style, 5, 8); //sets 4bytes desc_len, then 5bytes description (forced to "none" + 1byte null)
                } else {
                    APar_Unified_atom_Put(genericUUID, uuid_file_mimetype, UTF8_3GP_Style, mime_len, 8); //sets 1 byte mime len, then Xbytes mime type
                }

                FILE* uuid_binfile = APar_OpenFile(uuid_file_path, "rb");
                APar_Unified_atom_Put(genericUUID, NULL, UTF8_3GP_Style, file_len, 32);
                //store the data directly on the atom in AtomicData
                uint32_t bin_bytes_read = APar_ReadFile(genericUUID->AtomicData + (genericUUID->AtomicLength - 32), uuid_binfile, file_len);
                genericUUID->AtomicLength += bin_bytes_read;
                fclose(uuid_binfile);

            } else { //text type
                APar_Unified_atom_Put(genericUUID, argv[optind +1], UTF8_iTunesStyle_Unlimited, 0, 0);
            }

            break;
        }

        case Opt_Extract_all_uuids : {
            APar_ScanAtoms(ISObasemediafile);
            char* output_path = NULL;
            if (optind + 1 == argc) {
                output_path = argv[optind];
            }

            APar_OpenISOBaseMediaFile(ISObasemediafile, true);
            APar_Print_APuuid_atoms(ISObasemediafile, output_path, EXTRACT_ALL_UUID_BINARYS);
            APar_OpenISOBaseMediaFile(ISObasemediafile, false);

            exit(0);//never gets here
            break;
        }

        case Opt_Extract_a_uuid : {
            APar_ScanAtoms(ISObasemediafile);

            char* uuid_path = (char*)calloc(1, sizeof(char)*256+1);
            char* uuid_binary_str = (char*)calloc(1, sizeof(char)*20+1);
            char uuid_4char_name[16]; memset(uuid_4char_name, 0, 16);
            AtomicInfo* extractionAtom = NULL;

            UTF8Toisolat1((unsigned char*)&uuid_4char_name, 4, (unsigned char*)optarg, strlen(optarg) );
            APar_generate_uuid_from_atomname(uuid_4char_name, uuid_binary_str);

            //this will only append (and knock off) %s (anything) at the end of a string
            uint16_t path_len = strlen("moov.udta.meta.uuid=%s");
            memcpy(uuid_path, "moov.udta.meta.uuid=%s", path_len-2);
            memcpy(uuid_path + (path_len-2), uuid_binary_str, 16);

            extractionAtom = APar_FindAtom(uuid_path, false, EXTENDED_ATOM, 0, true);
            if (extractionAtom != NULL) {
                APar_OpenISOBaseMediaFile(ISObasemediafile, true);
                APar_Extract_uuid_binary_file(extractionAtom, ISObasemediafile, NULL);
                APar_OpenISOBaseMediaFile(ISObasemediafile, false);
            }

            free(uuid_path); uuid_path = NULL;
            free(uuid_binary_str); uuid_binary_str = NULL;
            exit(0);
            break; //never gets here
        }

        case Opt_Ipod_AVC_uuid : {
            if (deep_atom_scan == true) {
                if (strcmp(optarg, "1200") != 0) {
                    fprintf(stdout, "the ipod-uuid has a single preset of '1200' which is required\n"); //1200 might not be the only max macroblock setting down the pike
                    break;
                }
                uint8_t total_tracks = 0;
                uint8_t a_track = 0;//unused
                char atom_path[100];
                AtomicInfo* video_desc_atom = NULL;

                memset(atom_path, 0, 100);

                APar_FindAtomInTrack(total_tracks, a_track, NULL); //With track_num set to 0, it will return the total trak atom into total_tracks here.

                while (a_track < total_tracks) {
                    a_track++;
                    sprintf(atom_path, "moov.trak[%u].mdia.minf.stbl.stsd.avc1", a_track);
                    video_desc_atom = APar_FindAtom(atom_path, false, VERSIONED_ATOM, 0, false);

                    if (video_desc_atom != NULL) {
                        uint16_t mb_t = APar_TestVideoDescription(video_desc_atom, APar_OpenFile(ISObasemediafile, "rb"));
                        if (mb_t > 0 && mb_t <= 1200) {
                            sprintf(atom_path, "moov.trak[%u].mdia.minf.stbl.stsd.avc1.uuid=", a_track);
                            uint8_t uuid_baselen = (uint8_t)strlen(atom_path);
                            APar_uuid_scanf(atom_path + uuid_baselen, "6b6840f2-5f24-4fc5-ba39-a51bcf0323f3");
                            APar_endian_uuid_bin_str_conversion(atom_path + uuid_baselen);
                            APar_Generate_iPod_uuid(atom_path);
                        }
                    }
                }

            } else {
                fprintf(stdout, "the --DeepScan option is required for this operation. Skipping\n");
            }
            break;
        }

        case Manual_atom_removal : {
            APar_ScanAtoms(ISObasemediafile);

            char* compliant_name = (char*)malloc(sizeof(char)* strlen(optarg) +1);
            memset(compliant_name, 0, strlen(optarg) +1);
            UTF8Toisolat1((unsigned char*)compliant_name, strlen(optarg), (unsigned char*)optarg, strlen(optarg) );

            if (strstr(optarg, "uuid=") != NULL) {
                uint16_t uuid_path_pos = 0;
                uint16_t uuid_path_len = strlen(optarg);
                while (compliant_name+uuid_path_pos < compliant_name+uuid_path_len) {
                    if (strncmp(compliant_name+uuid_path_pos, "uuid=", 5) == 0) {
                        uuid_path_pos+=5;
                        break;
                    }
                    uuid_path_pos++;
                }
                if (strlen(compliant_name+uuid_path_pos) > 4) { //if =4 then it would be the deprecated form (or it should be, if not it just won't find anything; no harm done)
                    uint8_t uuid_len = APar_uuid_scanf(compliant_name+uuid_path_pos, optarg+uuid_path_pos);
                    compliant_name[uuid_path_pos+uuid_len] = 0;
                }
                APar_RemoveAtom(compliant_name, EXTENDED_ATOM, 0);

            } else if (strcmp(compliant_name + (strlen(compliant_name) - 4), "data") == 0) {
                APar_RemoveAtom(compliant_name, VERSIONED_ATOM, 0);

            } else {
                size_t string_len = strlen(compliant_name);
                //reverseDNS atom path
                if (strstr(optarg, ":[") != NULL && compliant_name[string_len-1] == ']' ) {
                    APar_RemoveAtom(compliant_name, VERSIONED_ATOM, 0);

                //packed language asset
                } else if (strncmp(compliant_name + string_len - 9, ":lang=", 6) == 0 ) {
                    uint16_t packed_lang = PackLanguage(compliant_name + string_len - 3, 0);
                    memset(compliant_name + string_len - 9, 0, 1);
                    APar_RemoveAtom(compliant_name, PACKED_LANG_ATOM, packed_lang);

                } else {
                    APar_RemoveAtom(compliant_name, UNKNOWN_ATOM, 0);
                }
            }
            free(compliant_name);
            compliant_name = NULL;
            break;
        }

        //3gp tags

        /*
        First, scan the file to get at atom tree (only happens once). Then take the cli args and look for optional arguments. All arguments begin with -- or -; other args
        are optional and are determined by directly testing arguments. Optional arguments common to the 3gp asset group (language, unicode, track/movie userdata) are
        extracted in find_optional_args. Setting assets in all tracks requires getting the number of tracks. Loop through either once (for movie & single track) or as
        many tracks there are for all tracks. Each pass through the loop, set the individual pieces of metadata.
        */
        case _3GP_Title : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style >= THIRD_GEN_PARTNER && metadata_style < MOTIONJPEG2000, 2, "title") ) {
                break;
            }
            bool set_UTF16_text = false;
            uint16_t packed_lang = 0;
            uint8_t userdata_area = MOVIE_LEVEL_ATOM;
            uint8_t selected_track = 0;
            uint8_t a_track = 0;//unused
            uint8_t asset_iterations = 0;

            find_optional_args(argv, optind, packed_lang, set_UTF16_text, userdata_area, selected_track, 3);

            if (userdata_area == MOVIE_LEVEL_ATOM || userdata_area == SINGLE_TRACK_ATOM) {
                asset_iterations = 1;
            } else if (userdata_area == ALL_TRACKS_ATOM) {
                APar_FindAtomInTrack(asset_iterations, a_track, NULL); //With asset_iterations set to 0, it will return the total trak atom into total_tracks here.
                if (asset_iterations == 1) selected_track = 1; //otherwise, APar_UserData_atom_Init will shift to non-existing track 0
            }

            for (uint8_t i_asset = 1; i_asset <= asset_iterations; i_asset++) {
                AtomicInfo* title_asset = APar_UserData_atom_Init("titl", optarg, userdata_area, asset_iterations == 1 ? selected_track : i_asset, packed_lang);
                APar_Unified_atom_Put(title_asset, optarg, (set_UTF16_text ? UTF16_3GP_Style : UTF8_3GP_Style), packed_lang, 16);
            }
            break;
        }

        case _3GP_Author : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style >= THIRD_GEN_PARTNER && metadata_style < MOTIONJPEG2000, 2, "author") ) {
                break;
            }
            bool set_UTF16_text = false;
            uint16_t packed_lang = 0;
            uint8_t userdata_area = MOVIE_LEVEL_ATOM;
            uint8_t selected_track = 0;
            uint8_t a_track = 0;//unused
            uint8_t asset_iterations = 0;

            find_optional_args(argv, optind, packed_lang, set_UTF16_text, userdata_area, selected_track, 3);

            if (userdata_area == MOVIE_LEVEL_ATOM || userdata_area == SINGLE_TRACK_ATOM) {
                asset_iterations = 1;
            } else if (userdata_area == ALL_TRACKS_ATOM) {
                APar_FindAtomInTrack(asset_iterations, a_track, NULL); //With asset_iterations set to 0, it will return the total trak atom into total_tracks here.
                if (asset_iterations == 1) selected_track = 1;
            }

            for (uint8_t i_asset = 1; i_asset <= asset_iterations; i_asset++) {
                AtomicInfo* author_asset = APar_UserData_atom_Init("auth", optarg, userdata_area, asset_iterations == 1 ? selected_track : i_asset, packed_lang);
                APar_Unified_atom_Put(author_asset, optarg, (set_UTF16_text ? UTF16_3GP_Style : UTF8_3GP_Style), packed_lang, 16);
            }
            break;
        }

        case _3GP_Performer : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style >= THIRD_GEN_PARTNER && metadata_style < MOTIONJPEG2000, 2, "performer") ) {
                break;
            }
            bool set_UTF16_text = false;
            uint16_t packed_lang = 0;
            uint8_t userdata_area = MOVIE_LEVEL_ATOM;
            uint8_t selected_track = 0;
            uint8_t a_track = 0;//unused
            uint8_t asset_iterations = 0;

            find_optional_args(argv, optind, packed_lang, set_UTF16_text, userdata_area, selected_track, 3);

            if (userdata_area == MOVIE_LEVEL_ATOM || userdata_area == SINGLE_TRACK_ATOM) {
                asset_iterations = 1;
            } else if (userdata_area == ALL_TRACKS_ATOM) {
                APar_FindAtomInTrack(asset_iterations, a_track, NULL); //With asset_iterations set to 0, it will return the total trak atom into total_tracks here.
                if (asset_iterations == 1) selected_track = 1;
            }

            for (uint8_t i_asset = 1; i_asset <= asset_iterations; i_asset++) {
                AtomicInfo* performer_asset = APar_UserData_atom_Init("perf", optarg, userdata_area, asset_iterations == 1 ? selected_track : i_asset, packed_lang);
                APar_Unified_atom_Put(performer_asset, optarg, (set_UTF16_text ? UTF16_3GP_Style : UTF8_3GP_Style), packed_lang, 16);
            }
            break;
        }

        case _3GP_Genre : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style >= THIRD_GEN_PARTNER && metadata_style < MOTIONJPEG2000, 2, "genre") ) {
                break;
            }
            bool set_UTF16_text = false;
            uint16_t packed_lang = 0;
            uint8_t userdata_area = MOVIE_LEVEL_ATOM;
            uint8_t selected_track = 0;
            uint8_t a_track = 0;//unused
            uint8_t asset_iterations = 0;

            find_optional_args(argv, optind, packed_lang, set_UTF16_text, userdata_area, selected_track, 3);

            if (userdata_area == MOVIE_LEVEL_ATOM || userdata_area == SINGLE_TRACK_ATOM) {
                asset_iterations = 1;
            } else if (userdata_area == ALL_TRACKS_ATOM) {
                APar_FindAtomInTrack(asset_iterations, a_track, NULL); //With asset_iterations set to 0, it will return the total trak atom into total_tracks here.
                if (asset_iterations == 1) selected_track = 1;
            }

            for (uint8_t i_asset = 1; i_asset <= asset_iterations; i_asset++) {
                AtomicInfo* genre_asset = APar_UserData_atom_Init("gnre", optarg, userdata_area, asset_iterations == 1 ? selected_track : i_asset, packed_lang);
                APar_Unified_atom_Put(genre_asset, optarg, (set_UTF16_text ? UTF16_3GP_Style : UTF8_3GP_Style), packed_lang, 16);
            }
            break;
        }

        case _3GP_Description : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style >= THIRD_GEN_PARTNER && metadata_style < MOTIONJPEG2000, 2, "description") ) {
                break;
            }
            bool set_UTF16_text = false;
            uint16_t packed_lang = 0;
            uint8_t userdata_area = MOVIE_LEVEL_ATOM;
            uint8_t selected_track = 0;
            uint8_t a_track = 0;//unused
            uint8_t asset_iterations = 0;

            find_optional_args(argv, optind, packed_lang, set_UTF16_text, userdata_area, selected_track, 3);

            if (userdata_area == MOVIE_LEVEL_ATOM || userdata_area == SINGLE_TRACK_ATOM) {
                asset_iterations = 1;
            } else if (userdata_area == ALL_TRACKS_ATOM) {
                APar_FindAtomInTrack(asset_iterations, a_track, NULL); //With asset_iterations set to 0, it will return the total trak atom into total_tracks here.
                if (asset_iterations == 1) selected_track = 1;
            }

            for (uint8_t i_asset = 1; i_asset <= asset_iterations; i_asset++) {
                AtomicInfo* description_asset = APar_UserData_atom_Init("dscp", optarg, userdata_area, asset_iterations == 1 ? selected_track : i_asset, packed_lang);
                APar_Unified_atom_Put(description_asset, optarg, (set_UTF16_text ? UTF16_3GP_Style : UTF8_3GP_Style), packed_lang, 16);
            }
            break;
        }

        case ISO_Copyright:       //ISO copyright atom common to all files that are derivatives of the base media file format, identical to....
        case _3GP_Copyright : {   //the 3gp copyright asset; this gets a test for major branding (but only with the cli arg --3gp-copyright).
            APar_ScanAtoms(ISObasemediafile);

            if (c == _3GP_Copyright) {
                if ( !APar_assert(metadata_style >= THIRD_GEN_PARTNER && metadata_style < MOTIONJPEG2000, 2, "copyright") ) {
                    break;
                }
            }

            bool set_UTF16_text = false;
            uint16_t packed_lang = 0;
            uint8_t userdata_area = MOVIE_LEVEL_ATOM;
            uint8_t selected_track = 0;
            uint8_t a_track = 0;//unused
            uint8_t asset_iterations = 0;

            find_optional_args(argv, optind, packed_lang, set_UTF16_text, userdata_area, selected_track, 3);

            if (userdata_area == MOVIE_LEVEL_ATOM || userdata_area == SINGLE_TRACK_ATOM) {
                asset_iterations = 1;
            } else if (userdata_area == ALL_TRACKS_ATOM) {
                APar_FindAtomInTrack(asset_iterations, a_track, NULL); //With asset_iterations set to 0, it will return the total trak atom into total_tracks here.
                if (asset_iterations == 1) selected_track = 1;
            }

            for (uint8_t i_asset = 1; i_asset <= asset_iterations; i_asset++) {
                AtomicInfo* copyright_notice = APar_UserData_atom_Init("cprt", optarg, userdata_area, asset_iterations == 1 ? selected_track : i_asset, packed_lang);
                APar_Unified_atom_Put(copyright_notice, optarg, (set_UTF16_text ? UTF16_3GP_Style : UTF8_3GP_Style), packed_lang, 16);
            }
            break;
        }

        case _3GP_Album : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style >= THIRD_GEN_PARTNER && metadata_style < MOTIONJPEG2000, 2, "album") ) {
                break;
            }
            bool set_UTF16_text = false;
            uint16_t packed_lang = 0;
            uint8_t userdata_area = MOVIE_LEVEL_ATOM;
            uint8_t selected_track = 0;
            uint8_t a_track = 0;//unused
            uint8_t asset_iterations = 0;
            uint8_t tracknum = 0;

            find_optional_args(argv, optind, packed_lang, set_UTF16_text, userdata_area, selected_track, 4);

            //cygle through the remaining independant arguments (before the next --cli_flag) and figure out if any are useful to us; already have lang & utf16
            for (int i= 0; i <= 4; i++) { //3 possible arguments for this tag (the first - which doesn't count - is the data for the tag itself)
                if ( argv[optind + i] && optind + i <= total_args) {
                    if ( strncmp(argv[optind + i], "trknum=", 7) == 0 ) {
                        char* track_num = argv[optind + i];
                        strsep(&track_num,"=");
                        tracknum = strtoul(track_num, NULL, 10);
                    }
                    if (*argv[optind + i] == '-') break;
                }
            }

            if (userdata_area == MOVIE_LEVEL_ATOM || userdata_area == SINGLE_TRACK_ATOM) {
                asset_iterations = 1;
            } else if (userdata_area == ALL_TRACKS_ATOM) {
                APar_FindAtomInTrack(asset_iterations, a_track, NULL); //With asset_iterations set to 0, it will return the total trak atom into total_tracks here.
                if (asset_iterations == 1) selected_track = 1;
            }

            for (uint8_t i_asset = 1; i_asset <= asset_iterations; i_asset++) {
                AtomicInfo* album_asset = APar_UserData_atom_Init("albm", optarg, userdata_area, asset_iterations == 1 ? selected_track : i_asset, packed_lang);
                APar_Unified_atom_Put(album_asset, optarg, (set_UTF16_text ? UTF16_3GP_Style : UTF8_3GP_Style), packed_lang, 16);
                if (tracknum != 0) {
                    APar_Unified_atom_Put(album_asset, NULL, UTF8_3GP_Style, tracknum, 8);
                }
            }
            break;
        }

        case _3GP_Year : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style >= THIRD_GEN_PARTNER && metadata_style < MOTIONJPEG2000, 2, "year") ) {
                break;
            }
            uint8_t userdata_area = MOVIE_LEVEL_ATOM;
            uint8_t selected_track = 0;
            uint8_t a_track = 0;//unused
            uint8_t asset_iterations = 0;
            uint16_t year_tag = 0;

            if ( argv[optind] && optind <= total_args) {
                if ( strcmp(argv[optind], "movie") == 0 ) {
                    userdata_area = MOVIE_LEVEL_ATOM;                   }
                if ( strncmp(argv[optind], "track=", 6) == 0 ) {
                    char* trak_idx = argv[optind];
                    strsep(&trak_idx, "=");
                    selected_track = strtoul(trak_idx, NULL, 10);
                    userdata_area = SINGLE_TRACK_ATOM;
                } else if ( strcmp(argv[optind], "track") == 0 ) {
                    userdata_area = ALL_TRACKS_ATOM;
                }
            }

            sscanf(optarg, "%" SCNu16, &year_tag);

            if (userdata_area == MOVIE_LEVEL_ATOM || userdata_area == SINGLE_TRACK_ATOM) {
                asset_iterations = 1;
            } else if (userdata_area == ALL_TRACKS_ATOM) {
                APar_FindAtomInTrack(asset_iterations, a_track, NULL); //With asset_iterations set to 0, it will return the total trak atom into total_tracks here.
                if (asset_iterations == 1) selected_track = 1;
            }

            for (uint8_t i_asset = 1; i_asset <= asset_iterations; i_asset++) {
                AtomicInfo* recordingyear_asset = APar_UserData_atom_Init("yrrc", optarg, userdata_area, asset_iterations == 1 ? selected_track : i_asset, 0);
                APar_Unified_atom_Put(recordingyear_asset, NULL, UTF8_3GP_Style, year_tag, 16);
            }
            break;
        }

        case _3GP_Rating : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style >= THIRD_GEN_PARTNER && metadata_style < MOTIONJPEG2000, 2, "rating") ) {
                break;
            }
            char rating_entity[5] = { 'N', 'O', 'N', 'E', 0 }; //'NONE' - thats what it will be if not provided
            char rating_criteria[5] = { 'N', 'O', 'N', 'E', 0 };
            bool set_UTF16_text = false;
            uint16_t packed_lang = 0;
            uint8_t userdata_area = MOVIE_LEVEL_ATOM;
            uint8_t selected_track = 0;
            uint8_t a_track = 0;//unused
            uint8_t asset_iterations = 0;

            find_optional_args(argv, optind, packed_lang, set_UTF16_text, userdata_area, selected_track, 5);

            for (int i= 0; i < 5; i++) { //3 possible arguments for this tag (the first - which doesn't count - is the data for the tag itself)
                if ( argv[optind + i] && optind + i <= total_args) {
                    if ( strncmp(argv[optind + i], "entity=", 7) == 0 ) {
                        char* entity = argv[optind + i];
                        strsep(&entity,"=");
                        memcpy(&rating_entity, entity, 4);
                    }
                    if ( strncmp(argv[optind + i], "criteria=", 9) == 0 ) {
                        char* criteria = argv[optind + i];
                        strsep(&criteria,"=");
                        memcpy(&rating_criteria, criteria, 4);
                    }
                    if (*argv[optind + i] == '-') break; //we've hit another cli argument
                }
            }

            if (userdata_area == MOVIE_LEVEL_ATOM || userdata_area == SINGLE_TRACK_ATOM) {
                asset_iterations = 1;
            } else if (userdata_area == ALL_TRACKS_ATOM) {
                APar_FindAtomInTrack(asset_iterations, a_track, NULL); //With asset_iterations set to 0, it will return the total trak atom into total_tracks here.
                if (asset_iterations == 1) selected_track = 1;
            }

            for (uint8_t i_asset = 1; i_asset <= asset_iterations; i_asset++) {
                AtomicInfo* rating_asset = APar_UserData_atom_Init("rtng", optarg, userdata_area, asset_iterations == 1 ? selected_track : i_asset, packed_lang);

                APar_Unified_atom_Put(rating_asset, NULL, UTF8_3GP_Style, UInt32FromBigEndian(rating_entity), 32);
                APar_Unified_atom_Put(rating_asset, NULL, UTF8_3GP_Style, UInt32FromBigEndian(rating_criteria), 32);
                APar_Unified_atom_Put(rating_asset, optarg, (set_UTF16_text ? UTF16_3GP_Style : UTF8_3GP_Style), packed_lang, 16);
            }
            break;
        }

        case _3GP_Classification : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style >= THIRD_GEN_PARTNER && metadata_style < MOTIONJPEG2000, 2, "classification") ) {
                break;
            }
            char classification_entity[5] = { 'N', 'O', 'N', 'E', 0 }; //'NONE' - thats what it will be if not provided
            uint16_t classification_index = 0;
            bool set_UTF16_text = false;
            uint16_t packed_lang = 0;
            uint8_t userdata_area = MOVIE_LEVEL_ATOM;
            uint8_t selected_track = 0;
            uint8_t a_track = 0;//unused
            uint8_t asset_iterations = 0;

            find_optional_args(argv, optind, packed_lang, set_UTF16_text, userdata_area, selected_track, 5);

            for (int i= 0; i < 4; i++) { //3 possible arguments for this tag (the first - which doesn't count - is the data for the tag itself)
                if ( argv[optind + i] && optind + i <= total_args) {
                    if ( strncmp(argv[optind + i], "entity=", 7) == 0 ) {
                        char* cls_entity = argv[optind + i];
                        strsep(&cls_entity, "=");
                        memcpy(&classification_entity, cls_entity, 4);
                    }
                    if ( strncmp(argv[optind + i], "index=", 6) == 0 ) {
                        char* cls_idx = argv[optind + i];
                        strsep(&cls_idx, "=");
                        sscanf(cls_idx, "%" SCNu16, &classification_index);
                    }
                    if (*argv[optind + i] == '-') break; //we've hit another cli argument
                }
            }

            if (userdata_area == MOVIE_LEVEL_ATOM || userdata_area == SINGLE_TRACK_ATOM) {
                asset_iterations = 1;
            } else if (userdata_area == ALL_TRACKS_ATOM) {
                APar_FindAtomInTrack(asset_iterations, a_track, NULL); //With asset_iterations set to 0, it will return the total trak atom into total_tracks here.
                if (asset_iterations == 1) selected_track = 1;
            }

            for (uint8_t i_asset = 1; i_asset <= asset_iterations; i_asset++) {
                AtomicInfo* classification_asset = APar_UserData_atom_Init("clsf", optarg, userdata_area, asset_iterations == 1 ? selected_track : i_asset, packed_lang);

                APar_Unified_atom_Put(classification_asset, NULL, UTF8_3GP_Style, UInt32FromBigEndian(classification_entity), 32);
                APar_Unified_atom_Put(classification_asset, NULL, UTF8_3GP_Style, classification_index, 16);
                APar_Unified_atom_Put(classification_asset, optarg, (set_UTF16_text ? UTF16_3GP_Style : UTF8_3GP_Style), packed_lang, 16);
            }
            break;
        }

        case _3GP_Keyword : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style >= THIRD_GEN_PARTNER && metadata_style < MOTIONJPEG2000, 2, "keyword") ) {
                break;
            }
            bool set_UTF16_text = false;
            uint16_t packed_lang = 0;
            uint8_t userdata_area = MOVIE_LEVEL_ATOM;
            uint8_t selected_track = 0;
            uint8_t a_track = 0;//unused
            uint8_t asset_iterations = 0;
            char* formed_keyword_struct = NULL;

            find_optional_args(argv, optind, packed_lang, set_UTF16_text, userdata_area, selected_track, 4);

            if (userdata_area == MOVIE_LEVEL_ATOM || userdata_area == SINGLE_TRACK_ATOM) {
                asset_iterations = 1;
            } else if (userdata_area == ALL_TRACKS_ATOM) {
                APar_FindAtomInTrack(asset_iterations, a_track, NULL); //With asset_iterations set to 0, it will return the total trak atom into total_tracks here.
                if (asset_iterations == 1) selected_track = 1;
            }

            if (strrchr(optarg, '=') != NULL) { //must be in the format of:   keywords=foo1,foo2,foo3,foo4
                char* arg_keywords = optarg;
                char* keywords_globbed = strsep(&arg_keywords,"="); //separate out 'keyword='
                keywords_globbed = strsep(&arg_keywords,"="); //this is what we want to work on: just the keywords
                char* keyword_ptr = keywords_globbed;
                uint32_t keyword_strlen = strlen(keywords_globbed);
                uint8_t keyword_count = 0;
                uint32_t key_index = 0;

                if (keyword_strlen > 0) { //if there is anything past the = then it counts as a keyword
                    keyword_count++;
                }

                while (true) { //count occurrences of comma here
                    if (*keyword_ptr == ',') {
                        keyword_count++;
                    }
                    keyword_ptr++;
                    key_index++;
                    if (keyword_strlen == key_index) {
                        break;
                    }
                }

                formed_keyword_struct = (char*)calloc(1, sizeof(char)* set_UTF16_text ? (keyword_strlen * 4) : (keyword_strlen * 2)); // *4 should carry utf16's BOM & TERM
                uint32_t keyword_struct_bytes = APar_3GP_Keyword_atom_Format(keywords_globbed, keyword_count, set_UTF16_text, formed_keyword_struct);

                for (uint8_t i_asset = 1; i_asset <= asset_iterations; i_asset++) {
                    AtomicInfo* keyword_asset = APar_UserData_atom_Init("kywd", keyword_strlen ? "temporary" : "", userdata_area, asset_iterations == 1 ? selected_track : i_asset, packed_lang); //just a "temporary" valid string to satisfy a test there
                    if (keyword_strlen > 0) {
                        APar_Unified_atom_Put(keyword_asset, NULL, UTF8_3GP_Style, packed_lang, 16);
                        APar_Unified_atom_Put(keyword_asset, NULL, UTF8_3GP_Style, keyword_count, 8);
                        APar_atom_Binary_Put(keyword_asset, formed_keyword_struct, keyword_struct_bytes, 3);
                    }
                }
                if (formed_keyword_struct != NULL) {
                    free(formed_keyword_struct);
                    formed_keyword_struct = NULL;
                }
            } else {
                for (uint8_t i_asset = 1; i_asset <= asset_iterations; i_asset++) {
                    APar_UserData_atom_Init("kywd", "", userdata_area, asset_iterations == 1 ? selected_track : i_asset, packed_lang);
                }
            }
            break;
        }

        case _3GP_Location : {
            APar_ScanAtoms(ISObasemediafile);
            if ( !APar_assert(metadata_style >= THIRD_GEN_PARTNER && metadata_style < MOTIONJPEG2000, 2, "location") ) {
                break;
            }
            bool set_UTF16_text = false;
            uint16_t packed_lang = 0;
            uint8_t userdata_area = MOVIE_LEVEL_ATOM;
            uint8_t selected_track = 0;
            uint8_t a_track = 0;//unused
            uint8_t asset_iterations = 0;
            double longitude = -73.98; //if you don't provide a place, you WILL be put right into Central Park. Welcome to New York City... now go away.
            double latitude = 40.77;
            double altitude = 4.3;
            uint8_t role = 0;
            const char* astronomical_body = "Earth";
            const char* additional_notes = "no notes";

            find_optional_args(argv, optind, packed_lang, set_UTF16_text, userdata_area, selected_track, 10);

            for (int i= 0; i <= 10; i++) { //9 possible arguments for this tag (the first - which doesn't count - is the data for the tag itself)
                if ( argv[optind + i] && optind + i <= total_args) {
                    if ( strncmp(argv[optind + i], "longitude=", 10) == 0 ) {
                        char* _long = argv[optind + i];
                        strsep(&_long,"=");
                        sscanf(_long, "%lf", &longitude);
                        if (_long[strlen(_long)-1] == 'W') {
                            longitude*=-1;
                        }
                    }
                    if ( strncmp(argv[optind + i], "latitude=", 9) == 0 ) {
                        char* _latt = argv[optind + i];
                        strsep(&_latt,"=");
                        sscanf(_latt, "%lf", &latitude);
                        if (_latt[strlen(_latt)-1] == 'S') {
                            latitude*=-1;
                        }
                    }
                    if ( strncmp(argv[optind + i], "altitude=", 9) == 0 ) {
                        char* _alti = argv[optind + i];
                        strsep(&_alti,"=");
                        sscanf(_alti, "%lf", &altitude);
                        if (_alti[strlen(_alti)-1] == 'B') {
                            altitude*=-1;
                        }
                    }
                    if ( strncmp(argv[optind + i], "role=", 5) == 0 ) {
                        char* _role = argv[optind + i];
                        strsep(&_role,"=");
                        if (strcmp(_role, "shooting location") == 0 || strcmp(_role, "shooting") == 0) {
                            role = 0;
                        } else if (strcmp(_role, "real location") == 0 || strcmp(_role, "real") == 0) {
                            role = 1;
                        } else if (strcmp(_role, "fictional location") == 0 || strcmp(_role, "fictional") == 0) {
                            role = 2;
                        }
                    }
                    if ( strncmp(argv[optind + i], "body=", 5) == 0 ) {
                        char* _astrobody = argv[optind + i];
                        strsep(&_astrobody,"=");
                        astronomical_body = _astrobody;
                    }
                    if ( strncmp(argv[optind + i], "notes=", 6) == 0 ) {
                        char* _add_notes = argv[optind + i];
                        strsep(&_add_notes,"=");
                        additional_notes = _add_notes;
                    }
                    if (*argv[optind + i] == '-') break; //we've hit another cli argument
                }
            }

            //fprintf(stdout, "long, lat, alt = %lf %lf %lf\n", longitude, latitude, altitude);
            if (userdata_area == MOVIE_LEVEL_ATOM || userdata_area == SINGLE_TRACK_ATOM) {
                asset_iterations = 1;
            } else if (userdata_area == ALL_TRACKS_ATOM) {
                APar_FindAtomInTrack(asset_iterations, a_track, NULL); //With asset_iterations set to 0, it will return the total trak atom into total_tracks here.
                if (asset_iterations == 1) selected_track = 1;
            }

            if (longitude < -180.0 || longitude > 180.0 || latitude < -90.0 || latitude > 90.0) {
                fprintf(stdout, "AtomicParsley warning: longitude or latitude was invalid; skipping setting location\n");
            } else {
                for (uint8_t i_asset = 1; i_asset <= asset_iterations; i_asset++) {
                    //short location_3GP_atom = APar_UserData_atom_Init("moov.udta.loci", optarg, packed_lang);
                    AtomicInfo* location_asset = APar_UserData_atom_Init("loci", optarg, userdata_area, asset_iterations == 1 ? selected_track : i_asset, packed_lang);
                    APar_Unified_atom_Put(location_asset, optarg, (set_UTF16_text ? UTF16_3GP_Style : UTF8_3GP_Style), packed_lang, 16);
                    APar_Unified_atom_Put(location_asset, NULL, false, role, 8);

                    APar_Unified_atom_Put(location_asset, NULL, false, float_to_16x16bit_fixed_point(longitude), 32);
                    APar_Unified_atom_Put(location_asset, NULL, false, float_to_16x16bit_fixed_point(latitude), 32);
                    APar_Unified_atom_Put(location_asset, NULL, false, float_to_16x16bit_fixed_point(altitude), 32);
                    APar_Unified_atom_Put(location_asset, astronomical_body, (set_UTF16_text ? UTF16_3GP_Style : UTF8_3GP_Style), 0, 0);
                    APar_Unified_atom_Put(location_asset, additional_notes, (set_UTF16_text ? UTF16_3GP_Style : UTF8_3GP_Style), 0, 0);
                }
            }
            break;
        }

        case Meta_ReverseDNS_Form : { //--rDNSatom "mv-ma" name=iTuneEXTC domain=com.apple.iTunes
            char* reverseDNS_atomname = NULL;
            char* reverseDNS_atomdomain = NULL;
            uint32_t rdns_atom_flags = AtomFlags_Data_Text;

            APar_ScanAtoms(ISObasemediafile);

            if ( !APar_assert(metadata_style == ITUNES_STYLE, 1, "reverse DNS form") ) {
                break;
            }

            for (int i= 0; i <= 5-1; i++) {
                if ( argv[optind + i] && optind + i <= argc ) {
                    if ( strncmp(argv[optind + i], "name=", 5) == 0 ) {
                        reverseDNS_atomname = argv[optind + i]+5;
                    } else if ( strncmp(argv[optind + i], "domain=", 7) == 0 ) {
                        reverseDNS_atomdomain = argv[optind + i]+7;
                    } else if ( strncmp(argv[optind + i], "datatype=", 9) == 0 ) {
                        sscanf(argv[optind + i]+9, "%" SCNu32, &rdns_atom_flags);
                    }
                    if (*argv[optind + i] == '-') {
                        break; //we've hit another cli argument
                    }
                }
            }

            if (reverseDNS_atomname == NULL) {
                fprintf(stdout, "AtomicParsley warning: no name for the reverseDNS form was found. Skipping.\n");

            } else if ((int)strlen(reverseDNS_atomname) != test_conforming_alpha_string(reverseDNS_atomname) ) {
                fprintf(stdout, "AtomicParsley warning: Some part of the reverseDNS atom name was non-conforming. Skipping.\n");

            } else if (reverseDNS_atomdomain == NULL) {
                fprintf(stdout, "AtomicParsley warning: no domain for the reverseDNS form was found. Skipping.\n");

            } else if (rdns_atom_flags != AtomFlags_Data_Text) {
                fprintf(stdout, "AtomicParsley warning: currently, only the strings are supported in reverseDNS atoms. Skipping.\n");

            } else {
                AtomicInfo* rDNS_data_atom = APar_reverseDNS_atom_Init(reverseDNS_atomname, optarg, &rdns_atom_flags, reverseDNS_atomdomain);
                APar_Unified_atom_Put(rDNS_data_atom, optarg, UTF8_iTunesStyle_Unlimited, 0, 0);
            }

            break;
        }

        case Meta_rDNS_rating : {
            const char* media_rating = Expand_cli_mediastring(optarg);
            uint32_t rDNS_data_flags = AtomFlags_Data_Text;

            APar_ScanAtoms(ISObasemediafile);

            if (media_rating != NULL || strlen(optarg) == 0) {
                AtomicInfo* rDNS_rating_data_atom = APar_reverseDNS_atom_Init("iTunEXTC", media_rating, &rDNS_data_flags, "com.apple.iTunes");
                APar_Unified_atom_Put(rDNS_rating_data_atom, media_rating, UTF8_iTunesStyle_Unlimited, 0, 0);
            }

            break;
        }

        case Meta_ID3v2Tag : {
            const char* target_frame_ID = NULL;
            uint16_t packed_lang = 0;
            uint8_t char_encoding = TE_UTF8; //utf8 is the default encoding
            char meta_container = 0-MOVIE_LEVEL_ATOM;
            bool multistring = false;
            APar_ScanAtoms(ISObasemediafile);

            //limit the files that can be tagged with meta.ID32 atoms. The file has to conform to the ISO BMFFv2 in order for a 'meta' atom.
            //This should exclude files branded as 3gp5 for example, except it doesn't always. The test is for a compatible brand (of a v2 ISO MBFF).
            //Quicktime writes some 3GPP files as 3gp5 with a compatible brand of mp42, so tagging works on these files. Not when you use timed text though.
            if ( !APar_assert(parsedAtoms[0].ancillary_data != 0 || (metadata_style >= THIRD_GEN_PARTNER_VER1_REL7 && metadata_style < MOTIONJPEG2000), 3, NULL) ) {
                break;
            }
            AdjunctArgs* id3args = (AdjunctArgs*)malloc(sizeof(AdjunctArgs));

            id3args->targetLang = NULL; //it will default later to "eng"
            id3args->descripArg = NULL;
            id3args->mimeArg = NULL;
            id3args->pictypeArg = NULL;
            id3args->ratingArg = NULL;
            id3args->dataArg = NULL;
            id3args->pictype_uint8 = 0;
            id3args->groupSymbol = 0;
            id3args->zlibCompressed = false;
            id3args->multistringtext = false;

            target_frame_ID = ConvertCLIFrameStr_TO_frameID(optarg);
            if (target_frame_ID == NULL) {
                target_frame_ID = optarg;
            }

            int frameType = FrameStr_TO_FrameType(target_frame_ID);
            if (frameType >= 0) {
                if (TestCLI_for_FrameParams(frameType, 0)) {
                    id3args->descripArg = find_ID3_optarg(argv, optind, "desc=");
                }
                if (TestCLI_for_FrameParams(frameType, 1)) {
                    id3args->mimeArg = find_ID3_optarg(argv, optind, "mimetype=");
                }
                if (TestCLI_for_FrameParams(frameType, 2)) {
                    id3args->pictypeArg = find_ID3_optarg(argv, optind, "imagetype=");
                }
                if (TestCLI_for_FrameParams(frameType, 3)) {
                    id3args->dataArg = find_ID3_optarg(argv, optind, "uniqueID=");
                }
                if (TestCLI_for_FrameParams(frameType, 4)) {
                    id3args->filenameArg = find_ID3_optarg(argv, optind, "filename=");
                }
                if (TestCLI_for_FrameParams(frameType, 5)) {
                    id3args->ratingArg = find_ID3_optarg(argv, optind, "rating=");
                }
                if (TestCLI_for_FrameParams(frameType, 6)) {
                    id3args->dataArg = find_ID3_optarg(argv, optind, "counter=");
                }
                if (TestCLI_for_FrameParams(frameType, 7)) {
                    id3args->dataArg = find_ID3_optarg(argv, optind, "data=");
                }
                if (TestCLI_for_FrameParams(frameType, 8)) {
                    id3args->dataArg = find_ID3_optarg(argv, optind, "data=");
                }
                if (*find_ID3_optarg(argv, optind, "compressed") == '1') {
                    id3args->zlibCompressed = true;
                }

                const char* groupsymbol = find_ID3_optarg(argv, optind, "groupsymbol=");
                if (groupsymbol[0] == '0' && groupsymbol[1] == 'x') {
                    id3args->groupSymbol = strtoul(groupsymbol, NULL, 16);
                    if (id3args->groupSymbol < 0x80 || id3args->groupSymbol > 0xF0) id3args->groupSymbol = 0;
                }
            }

            scan_ID3_optargs(argv, optind, id3args->targetLang, packed_lang, char_encoding, &meta_container, multistring);
            if (id3args->targetLang == NULL) id3args->targetLang = "eng";

            APar_OpenISOBaseMediaFile(ISObasemediafile, true); //if not already scanned, the whole tag for *this* ID32 atom needs to be read from file
            AtomicInfo* id32_atom = APar_ID32_atom_Init(target_frame_ID, meta_container, id3args->targetLang, packed_lang);

            if (strcmp(argv[optind + 0], "extract") == 0 && (strcmp(target_frame_ID, "APIC") == 0 || strcmp(target_frame_ID, "GEOB") == 0)) {
                if (id32_atom != NULL) {
                    APar_Extract_ID3v2_file(id32_atom, target_frame_ID, ISObasemediafile, NULL, id3args);
                    APar_OpenISOBaseMediaFile(ISObasemediafile, false);
                }
                exit(0);
            }

            APar_OpenISOBaseMediaFile(ISObasemediafile, false);
            APar_ID3FrameAmmend(id32_atom, target_frame_ID, argv[optind + 0], id3args, char_encoding);

            free(id3args);
            id3args = NULL;

            break;
        }

        //utility functions

        case Metadata_Purge : {
            APar_ScanAtoms(ISObasemediafile);
            APar_RemoveAtom("moov.udta.meta.ilst", SIMPLE_ATOM, 0);

            break;
        }

        case UserData_Purge : {
            APar_ScanAtoms(ISObasemediafile);
            APar_RemoveAtom("moov.udta", SIMPLE_ATOM, 0);

            break;
        }

        case foobar_purge : {
            APar_ScanAtoms(ISObasemediafile);
            APar_RemoveAtom("moov.udta.tags", UNKNOWN_ATOM, 0);

            break;
        }

        case Opt_FreeFree : {
            APar_ScanAtoms(ISObasemediafile);
            int free_level = -1;
            if (argv[optind]) {
                sscanf(argv[optind], "%i", &free_level);
            }
            APar_freefree(free_level);

            break;
        }

        case OPT_OverWrite : {
            alter_original = true;
            break;
        }

#if defined (_WIN32)
        case OPT_PreserveTimeStamps : {
            alter_original = true;
            preserve_timestamps = true;
            break;
        }
#endif

        case Meta_dump : {
            APar_ScanAtoms(ISObasemediafile);
            APar_OpenISOBaseMediaFile(ISObasemediafile, true);
            APar_MetadataFileDump(ISObasemediafile);
            APar_OpenISOBaseMediaFile(ISObasemediafile, false);

            APar_FreeMemory();
            exit(0);
        }

        case OPT_NoOptimize : {
            force_existing_hierarchy = true;
            break;
        }

        case OPT_OutputFile : {
            output_file = optarg;
            break;
        }

        } /* end switch */
    } /* end while */

    // after all the modifications are enacted on the tree in memory, THEN
    // write out the changes

    if (modified_atoms) {

#if defined (_WIN32)
            HANDLE hFile, hFileOut;
            FILETIME createTime, accessTime, writeTime;
            if (preserve_timestamps == true)
            {
                hFile = APar_OpenFileWin32(ISObasemediafile, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
                if (hFile != INVALID_HANDLE_VALUE)
                    {
                    GetFileTime(hFile,&createTime,&accessTime,&writeTime);
                    CloseHandle(hFile);
                    }
                else
                    {
                        fprintf (stdout,"\n Invalid HANDLE!");
                    }
            }
#endif

        APar_DetermineAtomLengths();
        APar_OpenISOBaseMediaFile(ISObasemediafile, true);
        APar_WriteFile(ISObasemediafile, output_file, alter_original);

#if defined (_WIN32)
            if (preserve_timestamps == true)
            {

                hFileOut = APar_OpenFileWin32(ISObasemediafile, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);

                if (hFileOut != INVALID_HANDLE_VALUE)
                    {
                    SetFileTime(hFileOut,&createTime,&accessTime,&writeTime);
                    CloseHandle(hFileOut);
                    }
            }
#endif


        if (!alter_original) {
            // The file was opened orignally as read-only; when it came time to
            // writeback into the original file, that FILE was closed, and a
            // new one opened with write abilities, so to close a FILE that no
            // longer exists would.... be retarded.
            APar_OpenISOBaseMediaFile(ISObasemediafile, false);
        }
    } else {
        if (ISObasemediafile != NULL && argc > 3 && !deep_atom_scan) {
            fprintf(stdout, "No changes.\n");
        }
    }
    APar_FreeMemory();
    return 0;
}

#if defined (_WIN32)

#if !defined (__CYGWIN__)

int wmain( int argc, wchar_t *arguments[])
{
    int return_val=0;
    uint16_t name_len = wcslen(arguments[0]);
    if (name_len >= 9 && _wcsicmp(arguments[0] + (name_len-9), L"-utf8.exe") == 0) {
        UnicodeOutputStatus = UNIVERSAL_UTF8;
    } else {
        UnicodeOutputStatus = WIN32_UTF16;
    }

    char **argv = (char**)calloc(argc + 1, sizeof(char*));

    // for native Win32 & full unicode support (well, cli arguments anyway),
    // take whatever 16bit unicode windows gives (utf16le), and convert
    // EVERYTHING that is sends to utf8; use those utf8 strings (mercifully
    // subject to familiar standby's like strlen) to pass around the program
    // like getopt_long to get cli options; convert from utf8 filenames as
    // required for unicode filename support on Windows using wide file
    // functions.
    for(int z=0; z < argc; z++) {
        uint32_t wchar_length = wcslen(arguments[z])+1;
        argv[z] = (char *)malloc(sizeof(char)*8*wchar_length );
        memset(argv[z], 0, 8*wchar_length);
        if (UnicodeOutputStatus == WIN32_UTF16) {
            UTF16LEToUTF8((unsigned char*) argv[z], 8*wchar_length, (unsigned char*) arguments[z], wchar_length*2);
        } else {
            strip_bogusUTF16toRawUTF8((unsigned char*) argv[z], 8*wchar_length, arguments[z], wchar_length );
        }
    }

    argv[argc] = NULL;

    return_val = real_main(argc, argv);

    for (int free_cnt=0; free_cnt<argc; free_cnt++) {
        free(argv[free_cnt]);
        argv[free_cnt]=NULL;
    }

    free(argv);
    argv=NULL;

    return return_val;
}

#ifdef __MINGW32__

int main()
{
    int argc;
    wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    return wmain(argc, argv);
}

#endif

#else // defined __CYGWIN__

int main( int argc, char *argv[])
{
    size_t name_len = strlen(argv[0]);
    if (name_len >= 5 &&
        (strcmp(argv[0] + (name_len-5), "-utf8") == 0 ||
         strcmp(argv[0] + (name_len-5), "-UTF8") == 0)) {
        UnicodeOutputStatus = UNIVERSAL_UTF8;
    } else {
        UnicodeOutputStatus = WIN32_UTF16;
    }
    return real_main(argc, argv);
}

#endif

#else

int main( int argc, char *argv[])
{
    return real_main(argc, argv);
}

#endif

/* vim:ts=4:sw=4:et:
 */
