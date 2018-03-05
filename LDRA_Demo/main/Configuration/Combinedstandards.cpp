/***-------------------------------------------------------------------------------------------------------------
***
*** File: Combinedstandards.cpp
***
*** This is an example of a standalone program that can be used to generate
*** standards that checks code conforms to Hungarian Notation and User Defined standards.
***
*** This program uses the TBglhapi to check code.
*** Full Example usuage of the TBglhapi is available in C:\LDRA_Toolsuite\TBglhapi\
*** All the example H Standards in this file are derived from the examples above.
***
***
*** Enabling User Standards Checking
*** -----------------------------
***  To enable the Standards Checking as part of analysis, the following INI file
***  flag needs to be set in the main language section.
***
***  Access to the INI File is available from LDRA Testbed via "View -> Tidy and Display Testbed.ini"
***  or via TBvision/LDRArules advanced context menu on a File/Set (hold ctrl).
***
***  [C/C++ LDRA Testbed] or [C/C++ LDRArules]
***  USER_STANDARDS_GENERATOR=C:\LDRA_Toolsuite\Userstandards_combined.exe
***
***  PLEASE NOTE: AS PER TBGLHAPI DOCUMENTATION, THE EXECUTABLE MUST BE RUN FROM THE LDRA INSTALLATION DIRECTORY
***
***
*** Enabling H Standards Checking
*** -----------------------------
***     Ensure your <lang>pen.dat contains the H Standard References as below.
***     This file for example is located in C:\LDRA_Toolsuite\C\Cpen for C etc.
***
***     If the <lang>pen.dat file does not contain these standards, consult the LDRA
***     Documentation for updating the File.  Any new standards appended to the file
***     must also update the total number of standards at the top of the file on the
***     first line.
***
***     818 00 1    0    1 H Global Variable does not conform to style g_<name>
***     819 00 1    0    2 H Class Member does not conform to style m_<name>
***     820 00 1    0    3 H Enum Element does not conform to style e_<name>
***     821 00 1    0    4 H Class Name does not conform to style C<name>
***     822 00 1    0    5 H Namespace Name does not conform to style N<name>
***     823 00 1    0    6 H Pointer does not conform to style p_<name>
***     824 00 1    0    7 H Enum Name does not conform to style E<name>
***     825 00 1    0    8 H Global Func Name does not conform to style <file>_<name>
***     826 00 1    0    9 H Global Var Name does not conform to style <file>_<name>
***     827 00 1    0   10 H Null Terminated String does not conform to style sz_<name>
***
*** Enabling Z Standards Checking
*** -----------------------------
***     Ensure your <lang>pen.dat contains the Z Standard References as below.
***     This file for example is located in C:\LDRA_Toolsuite\C\Cpen for C etc.
***
***     If the <lang>pen.dat file does not contain these standards, consult the LDRA
***     Documentation for updating the File.  Any new standards appended to the file
***     must also update the total number of standards at the top of the file on the
***     first line.
***
***     817 00 1    0    1 Z File exceeds required size.
***     818 00 1    0    2 Z Filename exceeds 8 characters.
***     819 00 1    0    3 Z Include name does not match source file name.
***     820 00 1    0    4 Z Source does not match layout template.
***     821 00 1    0    5 Z Hexadecimal number found.
***     822 00 1    0    6 Z Forbidden word found.
***
***
*** Editing this File
*** -----------------
*** The arguments passed to this program are fixed (see main below).
***
*** Standards violations are in the form of LDRA SIF 101 records.
***
*** Any 101 records generated must be of valid format, especially with
*** reference to the <lang>pen.dat and <lang>report.dat files
***
***
***-------------------------------------------------------------------------------------------------------------*/
#include <iostream>
#include <common_class_holder.hpp>
#include <glhwrapper_interface.h>
#include <tblinemapper_interface.h>


/*
 * Macros
 */
#define MAXSTR 520 /* Max String Size */
#define MESSAGE_SIZE 200
#define REPORT     /* Produces a .naming File in the workarea detailing any Standards that are violated */
#define HSTANDARD 'H'
#define ZSTANDARD 'Z'

/*
 * Enumeration
 */
typedef enum { Msa, Cmp, Dfl, Xrf, Inf } Phase_e;

/* Constants */
const char* GLOBAL_ENUMS = "Global Enums";
const char* GLOBAL_VARS = "Global Variables";
const char* PARAMETERS = "Parameters";

/*
 * Structures
 *
 *
 * struct input_data
 * Command line information - LDRA Internal - do not modify.
 */
struct input_data
{
  char* source_name;
  int module_id;
  char* glh_path;
  char* phases_string;
  char* workdir;
};

/*
 * struct sif_101_info
 * This structure can be used to store information required
 * for a type 101 SIF record - LDRA Internal - do not modify.
 */
struct sif_101_info
{
   int module_id_101;
   int procedure_number;      /* From type 100 SIF record, 0 for whole file */
   int ref_code_line_number;  /* 0 for file or procedure as a whole */
   int penfile_index;         /* Value Column 1 in <lang>pen.dat - not used*/
   int user_standard_number;  /* Must match <n> H/Z in <lang>pen.dat and <lang>report.dat */
   char standard_text[200];   /* text to go in 101 record, either same as in pen file
                                 or additional information such as the file name */
};


/*
 * H Standard - User Editable Prototypes
 */
void append_standards_violations (const struct input_data* data, IToolSuiteInterface * iObj);

/* Check Globals 1 H */
void check_globals(const struct input_data * data,
                   FILE * fp,
                   IToolSuiteInterface * iObj);

/* Check Class Members 2 H */
void check_class_members (const struct input_data * data,
                          FILE * fp,
                          IToolSuiteInterface * iObj);

/* Check Enums 3 H, 7 H */
void check_enums(const struct input_data * data,
                 FILE * fp,
                 IToolSuiteInterface * iObj);

/* Check Classes 4 H */
void check_classes(const struct input_data * data,
                   FILE * fp,
                   IToolSuiteInterface * iObj);

/* Check Namespaces 5 H */
void check_namespaces(const struct input_data * data,
                      FILE * fp,
                      IToolSuiteInterface * iObj);

/* Check Pointer Parameters 6 H */
void check_pointers(const struct input_data * data,
                    FILE * fp,
                    IToolSuiteInterface * iObj);

/* Check Global Functions 8 H */
void check_global_functions(const struct input_data * data,
                            FILE * fp,
                            IToolSuiteInterface * iObj);

/* Check Global Vars 9 H */
void check_global_vars(const struct input_data * data,
                            FILE * fp,
                            IToolSuiteInterface * iObj);

/* Check Null Terminated String 10 H */
void check_null_terminated_strings(const struct input_data * data,
                            FILE * fp,
                            IToolSuiteInterface * iObj);


/*
 * Z Standard - User Editable Prototypes
 */
/* 1 Z: File exceeds required size. */
void check_file_size (const struct input_data* data, FILE* fp);

/* 2 Z: Checks filename size is <= 8 characters */
void check_filename_size (const struct input_data* data, FILE* fp);

/* 3 Z: Checks include filename is the same as the source filename */
void check_includename (const struct input_data* data, FILE* fp, char * line1);

/* 4 Z: Checks that layout templates are met */
void check_templates (const struct input_data* data, FILE* fp, char * line);

/* 5 Z: Checks that there is no use of hexadecimal radix */
void check_radix (const struct input_data* data, FILE* fp, char * line);

/* 6 Z: Checks that there is no use of forbidden words */
void check_forbidden_words (const struct input_data* data, FILE* fp, char * line, int linenum);


/*
 * LDRA Prototypes
 */
void append_violation (const struct input_data* data,
                       FILE* fp,
                       const int std_num,
                       const char *message,
                       const int linenumber,
                       const char std_letter);

void get_sif_sub_file_name (const struct input_data* data, Phase_e phase_id,
                            char* sfn_name);

void split_directory_path (const char *file_name,char *reduced_name,
                            char *directory_name);

void print_101_struct (FILE* fp, const struct sif_101_info* std, const char std_letter);

void process_naming_conformance (const struct input_data * data,
                                  FILE * fp,
                                  const char* type,
                                  const char* style,
                                  const char* name,
                                  const int conforms,
                                  const int linenumber,
                                  const int std_num);

void  print_to_report (const char* message, const int linenumber, const struct input_data* data);

long fsize (const char *infile);



/***----------------------------------------------------------------------
***
*** Routine: main
***
*** Parameters:
*** ===========
***
*** Action  Name                      Type
*** ------  ----                      ----
*** I       argc                      int
*** I       argv                      char * []
***
*** Arguments provided by LDRA Application are:
*** =======================================
***
*** <full source file name>     "C:\LDRA_Workarea\Examples\Testrian.c"
*** <module ID>                 1
*** <full glh path>             "C:\LDRA_Workarea\tbwrkfls\Testrian_1.glh"
*** <phases run codes>          "MSA CMP DFL XRF"
***
***----------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
  int return_code = 0;
  const int VALID_ARGC = 5;
  char filename[MAXSTR] = {0};
  char workfilesdir[MAXSTR] = {0};
  struct input_data input_d;

  try
  {
     /* Check the correct number of arguments are provided */
      if (argc == VALID_ARGC)
      {
         /* Full Source File Path */
         input_d.source_name = argv[1];
         /* Source File Module Number as int */
         input_d.module_id = atoi(argv[2]);
         /* Full GLH File Path */
         input_d.glh_path = argv[3];
         /* Phases Executed */
         input_d.phases_string = argv[4];

         /* Extract the workfiles directory from the GLH Path */
         split_directory_path(input_d.glh_path, filename, workfilesdir);
         /* Source LDRA Workfiles Directory */
         input_d.workdir = workfilesdir;


          /* Connect to the GLH API */
          CGLHWrapperClass wrapperInterface;
          IToolSuiteInterface * interfaceObj = wrapperInterface.getCreatedIEntityTool();

         /* If a connection is made, load the Results for the GLH File */
          if (interfaceObj != (IToolSuiteInterface*)(0))
          {
              if (!interfaceObj->loadResults(input_d.glh_path))
              {
                  throw "Failed to load GLH File";
              }

              /* Begin checking for H/Z Standard Violations now the GLH API is initialised */
              append_standards_violations (&input_d, interfaceObj);
          }
          else
          {
              throw "Failed to load GLH Interface Object, are you running from the LDRA Toolsuite directory?";
          }
      }
      else
      {
         printf ("Invalid number of arguments\n"\
                 "Arguments are: <full source name> <module number> <glh path> <phases run - MSA CMP...>\n");
         return_code = 1;
      }
  }
  catch(const char * msg)
  {
      printf("%s\n", msg);
      return_code = 1;
  }
  catch(...)
  {
      printf("Unknown Error Occurred\n");
      return_code = 1;
  }


#ifdef REPORT
  void close_report ();
  close_report ();
#endif


  return return_code;
} /* End of main */



/***----------------------------------------------------------------
***
*** Routine: append_standards_violations
***
*** Parameters:
*** ===========
***
*** Action  Name                      Type            Use
*** ------  ----                      ----            ---
*** I       data                      const struct input_data *
***
*** Description:
*** ============
***
*** Shows example of how to determine the appropriate SIF sub file
*** (e.g .sf1 for main static analysis) and append a 101 record to it.
***
*** Each of the functions to check the appropriate H Standards are called
*** from this function.
***
***----------------------------------------------------------------*/
void append_standards_violations (const struct input_data* data, IToolSuiteInterface * iObj)
{
   FILE * fp = NULL;
   char sfn_name[MAXSTR] = {0};

   /* Check that Main Static Analysis has been completed for this File */
   if (strstr (data->phases_string, "MSA") != NULL)
   {
      /*
      * Main Static Analysis has been run,
      * Generate any standards associated with this phase
      */
      get_sif_sub_file_name (data, Msa, sfn_name);

      /* Open the SIF File */
      fp = fopen(sfn_name,"a");

      if (fp)
      {
         /* Start H Standards Checking *******************************************/
         /* Check Globals 1 H */
         check_globals(data, fp, iObj);

         /* Check Class Members 2 H */
         check_class_members(data, fp, iObj);

         /* Check Enums 3 H, 7 H */
         check_enums(data, fp, iObj);

         /* Check Classes 4 H */
         check_classes(data, fp, iObj);

         /* Check Namespaces 5 H */
         check_namespaces(data, fp, iObj);

         /* Check Pointer Parameters 6 H */
         check_pointers(data, fp, iObj);

         /* Check Global Function Names 8 H */
         check_global_functions(data, fp, iObj);

         /* Check Global Var Names 9 H */
         check_global_vars(data, fp, iObj);

         /* Check Global Function Names 10 H */
//         check_null_terminated_strings(data, fp, iObj);
         /* End H Standards Checking *********************************************/


         /* Start Z Standards Checking *******************************************/
         /* Check File Size 1 Z */
         check_file_size(data, fp);

         /* 2 Z: Checks filename size is <= 8 characters */
         check_filename_size (data, fp);

         /* The following checks are performed on the source, line by line */
         char line[MAXSTR];
         FILE *source_fp = fopen(data->source_name, "r");
         if (source_fp == NULL)
         {
            printf("Source file %s not found\n", data->source_name);
         }
         else
         {
            int end = 0;
            int linenum = 1;
            while( !end )
            {
               memset(line, 0, sizeof(line));

               if (fgets(line, MAXSTR, source_fp) != NULL )
               { /* remove newline from end of string */
                  line[strlen(line)-1] = '\0';

                  /* 3 Z: Checks include filename is the same as the source filename */
                  check_includename (data, fp, line);

                  /* 4 Z: Checks that layout templates are met */
                  check_templates (data, fp, line);

                  /* 5 Z: Checks that there is no use of hexadecimal radix */
                  check_radix (data, fp, line);

                  /* 6 Z: Checks that there is no use of forbidden words */
                  check_forbidden_words (data, fp, line, linenum);
                  linenum++;
               }
               else
               {
                  end = 1;
               }
            }
         }
         fclose(source_fp);
         /* End Z Standards Checking *********************************************/


         fclose(fp);
      }
   }
} /* End of append_standards_violations */





/***----------------------------------------------------------------
***
*** Start of Standards
***
***----------------------------------------------------------------*/

/***----------------------------------------------------------------
***
*** Routine: check_globals
***
*** Parameters:
*** ===========
***
*** Action  Name             Type                           Use
*** ------  ----             ----                           ---
*** I       data             const struct input_data *      Argument Data
*** I       fp               FILE *                         SIF File pointer
*** I       iObj             IToolSuiteInterface *          GLH API Interface
***
*** Description:
*** ============
***
*** Checks that global variables in a File conform to H Standard g_<name>
***
*** Standards:
*** ==========
***
*** 1 H Global Variable does not conform to style g_<name>
***
***-----------------------------------------------------------------*/
void check_globals(const struct input_data * data,
                   FILE * fp,
                   IToolSuiteInterface * iObj)
{
   NLDRAAnalysis::CVariableTypeDeclaration globalVariableHolder;

   /* Load Global Variable results for File */
   iObj->loadGlobalComponent(data->source_name, GLOBAL_VARS, FileResults);


   /* Loop through each global variable found and check conformation with the Penalty */
   while(iObj->readGlobalComponent(globalVariableHolder))
   {
      char globalVariableName[MAXSTR] = {0};
      int conforms = 1;
      const int globalVariableLineNumber = globalVariableHolder.getRefStartLine();
      strcpy (globalVariableName, globalVariableHolder.getName());

      /* Perform the check */
      if(globalVariableName[0] != 'g' || globalVariableName[1] != '_')
      {
         conforms = 0;
      }

      /* Record the Result */
      process_naming_conformance (data, fp,
                                   "Global",
                                   "g_<name>",
                                   globalVariableName, conforms, globalVariableLineNumber,
                                   1);
   }
} /* End of check_globals */


/*
 * fullName = "C:\LDRA\LDRA_Demos\TICCS50_Workspace\TICCS50_TMS320F28335_Dice\dice.c"
 * return just the name ex: "dice"
 */
void getFileName( char* aName, const char* fullName ) {
	int dot_pos=0;
	int slash_pos=0;

	/* Find the last position of the dot and slash */
	int iter=0;
	while ( iter < strlen(fullName) ) {
		if ( fullName[iter] == '.' ) {
			dot_pos = iter;
		}
		if ( (fullName[iter] == '//') || (fullName[iter] == '\\') ) {
			slash_pos = iter;
		}
		iter++;
	}
	/* Return name */
	if ( dot_pos > slash_pos ) {
		strncpy ( aName, &fullName[slash_pos+1], dot_pos-slash_pos-1); 
	}
}


/***----------------------------------------------------------------
***
*** Routine: check_global_vars
***
*** Parameters:
*** ===========
***
*** Action  Name             Type                           Use
*** ------  ----             ----                           ---
*** I       data             const struct input_data *      Argument Data
*** I       fp               FILE *                         SIF File pointer
*** I       iObj             IToolSuiteInterface *          GLH API Interface
***
*** Description:
*** ============
***
*** Checks that global variables in a File conform to H Standard <file>_<name>
***
*** Standards:
*** ==========
***
*** 9 H Global Var does not conform to style <file>_<name>
***
***-----------------------------------------------------------------*/

void check_global_vars (const struct input_data * data, FILE * fp, IToolSuiteInterface * iObj) {
   NLDRAAnalysis::CVariableTypeDeclaration globalVariableHolder;
   char expectedName[MAXSTR] = {0};

   /* Load Global Variable results for File */
   iObj->loadGlobalComponent(data->source_name, GLOBAL_VARS, FileResults);

   /* Get the expected name for all global variables in this file */
   getFileName (expectedName, data->source_name);
   strcat (expectedName, "_");
 
   /* Loop through each global variable found and check conformation with the Penalty */
   while (iObj->readGlobalComponent(globalVariableHolder)) {
      char globalVariableName[MAXSTR] = {0};

      /* Only find the variables that are not static */
	  if (globalVariableHolder.getStaticField() == false) {
		  int conforms = 1;
		  const int globalVariableLineNumber = globalVariableHolder.getRefStartLine();
		  strcpy (globalVariableName, globalVariableHolder.getName());

		  /* This is a global var, so check that it conforms to <file>_<name> */
		  conforms = !strncmp (expectedName, globalVariableHolder.getName(), strlen(expectedName) );

		  /* Record the Result */
		  process_naming_conformance (data, fp, "Global var", "<file>_<name>",
									  globalVariableName, conforms, globalVariableLineNumber, 9);
	  }
   }
} /* End of check_global_vars */



/***----------------------------------------------------------------
***
*** Routine: check_class_members
***
*** Parameters:
*** ===========
***
*** Action  Name            Type                            Use
*** ------  ----            ----                            ---
*** I       data            const struct input_data *       Argument Data
*** I       fp              FILE *                          SIF File Pointer
*** I       iObj            IToolSuiteInterface *           GLH API Interface
***
*** Description:
*** ============
***
*** Checks that Class Member variables conform to H Standard m_<name>
***
*** Standards:
*** ==========
***
*** 2 H Class Member does not conform to style m_<name>
***
***-----------------------------------------------------------------*/
void check_class_members (const struct input_data * data,
                          FILE * fp,
                          IToolSuiteInterface * iObj)
{
   /* Load Class Information from GLH API Interface */
   if (iObj->loadAllClasses(false) > 0) /* Set Parameter to true to include Child Classes */
   {
      NLDRAAnalysis::CClassDeclaration classDeclarationHolder;
      /* Loop through classes available */
      while(iObj->readAllClassList(classDeclarationHolder))
      {
         NLDRAAnalysis::CMemberVariableDeclaration classMemberHolder;
         /* Load class member variables from API */
         iObj->loadClassMemberVariables(classDeclarationHolder);

         while(iObj->readMemberVariableList(classMemberHolder))
         {
            if (classMemberHolder.sourceFile().getModuleNumber() == data->module_id)
            {
               char classMemberVariableName[MAXSTR] = {0};
               int conforms = 1;
               const int classMemberLineNumber = classMemberHolder.getRefStartLine();
               strcpy(classMemberVariableName, classMemberHolder.getName());

               /* Perform the check */
               if(classMemberVariableName[0] != 'm' || classMemberVariableName[1] != '_')
               {
                  conforms = 0;
               }

               /* Record the Result */
               process_naming_conformance (data,
                                              fp,
                                              "Member",
                                              "m_<name>",
                                              classMemberVariableName,
                                              conforms,
                                              classMemberLineNumber,
                                              2);
            }
         }
      }
   }
} /* End of check_class_members */



/***----------------------------------------------------------------
***
*** Routine: check_enums
***
*** Parameters:
*** ===========
***
*** Action  Name          Type                          Use
*** ------  ----          ----                          ---
*** I       data          const struct input_data *     Argument Data
*** I       fp            FILE *                        SIF File Pointer
*** I       iObj          IToolSuiteInterface *         GLH API Interface
***
*** Description:
*** ============
***
*** Checks the Enum Name conforms to style E<name>.
*** It then checks all the elements of the enum that they conform to style e_<name>
***
*** Standards:
*** ==========
***
*** 3 H Enum Element does not conform to style e_<name>
*** 7 H Enum Name does not conform to style E<name>
***
***-----------------------------------------------------------------*/
void check_enums(const struct input_data * data,
                 FILE * fp,
                 IToolSuiteInterface * iObj)
{
   NLDRAAnalysis::CVariableListDeclaration enumListHolder;

   /* Load Global Enum component results for File */
   iObj->loadGlobalComponent(data->source_name, GLOBAL_ENUMS, FileResults);


   while(iObj->readGlobalComponentList(enumListHolder))
   {
      /* Check Penalty 7 H - Conformance of the Enum Name*/
      char enumName[MAXSTR] = {0};
      int conforms = 1;
      const int enumNameLineNumber = enumListHolder.getRefStartLine();
      strcpy(enumName, enumListHolder.getName());

      /* Perform the check */
      if(enumName[0] != 'E')
      {
         conforms = 0;
      }

      /* Record the Result */
      process_naming_conformance (data, fp,
                                       "Enum name",
                                       "E<name>",
                                       enumName, conforms, enumNameLineNumber,
                                       7);



      /* Check Penalty 3 H - Conformance of the Enum Elements*/
       NLDRAAnalysis::CVariableTypeDeclaration enumElementHolder;

       while (iObj->readGlobalComponentSubList(enumElementHolder))
       {
           char enumElementName[MAXSTR] = {0};
           int conforms = 1;
           const int enumElementLineNumber = enumElementHolder.getRefStartLine();
           strcpy(enumElementName, enumElementHolder.getName());

           /* Perform the check */
           if(enumElementName[0] != 'e' && enumElementName[1] != '_')
           {
              conforms = 0;
           }

           /* Record the Result */
           process_naming_conformance (data, fp,
                                       "Enum element",
                                       "e_<name>",
                                       enumElementName, conforms, enumElementLineNumber,
                                       3);
       }
   }
} /* End of check_enums */


/***----------------------------------------------------------------
***
*** Routine: check_null_terminated_strings
***
*** Parameters:
*** ===========
***
*** Action  Name          Type                          Use
*** ------  ----          ----                          ---
*** I       data          const struct input_data *     Argument Data
*** I       fp            FILE *                        SIF File Pointer
*** I       iObj          IToolSuiteInterface *         GLH API Interface
***
*** Description:
*** ============
***
*** Checks the Name of a null terminated string conforms to style sz_<name>.
***
*** Standards:
*** ==========
***
*** 10 H Null Terminated String Name does not conform to style sz_<name>
***
***-----------------------------------------------------------------*/
void check_null_terminated_strings(const struct input_data * data,
                 FILE * fp,
                 IToolSuiteInterface * iObj)
{
   NLDRAAnalysis::CVariableTypeDeclaration parameterHolder;
   NLDRAAnalysis::CFunctionDeclaration functionDeclarationHolder;
   NLDRAAnalysis::CVariableTypeDeclaration globalVariableHolder;
   
   /* Load Function data for Source File from GLH API */
   iObj->loadFunctions(data->source_name, FileResults);

   /* Load Global Variables component results for File */
   iObj->loadGlobalComponent(data->source_name, GLOBAL_VARS, FileResults);

   /* Loop through each global variable found and check conformation with the Penalty */
   while(iObj->readGlobalComponent(globalVariableHolder))
   {
      char globalVariableName[MAXSTR] = {0};
      int conforms = 1;
      const int globalVariableLineNumber = globalVariableHolder.getRefStartLine();
      strcpy (globalVariableName, globalVariableHolder.getName());

      /* Check if the global is a null terminated string */

      /* Perform the check */
      if( (globalVariableName[0] != 's') || (globalVariableName[1] != 'z') || (globalVariableName[2] != '_') )
      {
         conforms = 0;
      }

      /* Record the Result */
      process_naming_conformance (data, fp,
                                   "Null Terminated String",
                                   "sz_<name>",
                                   globalVariableName, conforms, globalVariableLineNumber,
                                   10);
   }
} /* End of check_null_terminated_strings */



/***----------------------------------------------------------------
***
*** Routine: check_classes
***
*** Parameters:
*** ===========
***
*** Action  Name          Type                          Use
*** ------  ----          ----                          ---
*** I       data          const struct input_data *     Argument Data
*** I       fp            FILE *                        SIF File Pointer
*** I       iObj          IToolSuiteInterface *         GLH API Interface
***
*** Description:
*** ============
***
*** Checks Class Names conform to H Standard C<name>
***
*** Standards:
*** ==========
***
*** 4 H Class Name does not conform to style C<name>
***
***-----------------------------------------------------------------*/

void check_classes(const struct input_data * data,
                   FILE * fp,
                   IToolSuiteInterface * iObj)
{
   /* Load Class Information from GLH API Interface */
   if (iObj->loadAllClasses(false) > 0) /* Set Parameter to true to include Child Classes */
   {
      NLDRAAnalysis::CClassDeclaration classDeclarationHolder;
      /* Loop through classes available */
      while(iObj->readAllClassList(classDeclarationHolder))
      {
         if (classDeclarationHolder.sourceFile().getModuleNumber() == data->module_id)
         {
            char fullClassName[MAXSTR] = {0};
            char className[MAXSTR] = {0};

            int conforms = 1;
            const int classLineNumber = classDeclarationHolder.getRefStartLine();
            strcpy(fullClassName, classDeclarationHolder.getName());

            /* Class name is fully qualified, to validate it we need to get just the class name. */
            int pos = strlen(fullClassName);
            pos = (strlen(fullClassName)-1);
            if(pos > -1)
            {
               while(pos > -1
                     &&
                     fullClassName[pos] != ':')
               {
                  pos--;
               }
               strcpy(className, &fullClassName[pos+1]);

               /* Perform the check */
               if(className[0] != 'C')
               {
                  conforms = 0;
               }

               /* Record the Result */
               process_naming_conformance (data, fp,
                                           "Class Name",
                                           "C<name>",
                                           className, conforms, classLineNumber,
                                           4);
            }
         }
      }
   }
} /* End of check_classes */



/***----------------------------------------------------------------
***
*** Routine: check_namespaces
***
*** Parameters:
*** ===========
***
*** Action  Name          Type                          Use
*** ------  ----          ----                          ---
*** I       data          const struct input_data *     Argument Data
*** I       fp            FILE *                        SIF File Pointer
*** I       iObj          IToolSuiteInterface *         GLH API Interface
***
*** Description:
*** ============
***
*** Checks Namespace names conform to H Standard N<name>
***
*** Standards:
*** ==========
***
*** 5 H Namespace Name does not conform to style N<name>
***
***-----------------------------------------------------------------*/
void check_namespaces(const struct input_data * data,
                      FILE * fp,
                      IToolSuiteInterface * iObj)
{
   /* Load Namespace Information from GLH API Interface */
   if (iObj->loadAllNamespaces() > 0)
   {
      NLDRAAnalysis::CNamespaceDeclaration namespaceHolder;

      /* Loop through Namespaces available */
      while(iObj->readAllNamespaceList(namespaceHolder))
      {
         char namespaceName[MAXSTR] = {0};
         int conforms = 1;
         strcpy(namespaceName, namespaceHolder.getName());

         /* Perform the check */
         if(namespaceName[0] != 'N')
         {
            conforms = 0;
         }

         /* Record the Result */
         process_naming_conformance (data, fp,
                                      "Namespace Name",
                                      "N<name>",
                                      namespaceName, conforms, -1,
                                      5);
      }
   }
} /* End of check_namespaces */



/***----------------------------------------------------------------
***
*** Routine: check_pointers
***
*** Parameters:
*** ===========
***
*** Action  Name          Type                          Use
*** ------  ----          ----                          ---
*** I       data          const struct input_data *     Argument Data
*** I       fp            FILE *                        SIF File Pointer
*** I       iObj          IToolSuiteInterface *         GLH API Interface
***
*** Description:
*** ============
***
*** Checks that Pointer Parameters conform to the style rule.
*** The same principle can be applied for Local variables etc.
***
*** Standards:
*** ==========
***
*** 6 H Pointer does not conform to style p_<name>
***
***-----------------------------------------------------------------*/
void check_pointers(const struct input_data * data,
                    FILE * fp,
                    IToolSuiteInterface * iObj)
{
   NLDRAAnalysis::CVariableTypeDeclaration parameterHolder;
   NLDRAAnalysis::CFunctionDeclaration functionDeclarationHolder;

   /* Load Function data for Source File from GLH API */
   iObj->loadFunctions(data->source_name, FileResults);


   /* Loop over each function in the file */
   while(iObj->readFunctionList(functionDeclarationHolder))
   {
      /* Load the Parameter information for the function from the GLH API */
      iObj->loadComponents(functionDeclarationHolder,
         PARAMETERS,
         FunctionResults,
         data->source_name);

      /* Loop over each parameter for the function */
      while(iObj->readParameterComponent(parameterHolder))
      {
         /* Check if the parameter is a pointer */
         if(parameterHolder.getIsPointer())
         {
            char pointerParameterName[MAXSTR] = {0};
            int conforms = 1;
            int parameterLineNumber = parameterHolder.getRefStartLine();
            strcpy(pointerParameterName, parameterHolder.getName());

            /* Check if the Line Number for the Parameter is provided, fallback to the function if not */
            if (parameterLineNumber < 1)
            {
               parameterLineNumber = functionDeclarationHolder.getRefStartLine();
            }

            /* Perform the check */
            if(pointerParameterName[0] != 'p' || pointerParameterName[1] != '_')
            {
               conforms = 0;
            }

            /* Record the Result */
            process_naming_conformance (data, fp,
               "Pointer",
               "p_<name>",
               pointerParameterName, conforms, parameterLineNumber,
               6);
         }
      }
   }
} /* End of check_pointers */


/***----------------------------------------------------------------
***
*** Routine: check_global_functions
***
*** Parameters:
*** ===========
***
*** Action  Name          Type                          Use
*** ------  ----          ----                          ---
*** I       data          const struct input_data *     Argument Data
*** I       fp            FILE *                        SIF File Pointer
*** I       iObj          IToolSuiteInterface *         GLH API Interface
***
*** Description:
*** ============
***
*** Checks that each Global Function conform to the style rule.
*** The same principle can be applied for Local variables etc.
***
*** Standards:
*** ==========
***
*** 8 H Global Function Name does not conform to style <file>_<name>
***
***-----------------------------------------------------------------*/
void check_global_functions(const struct input_data * data, FILE * fp, IToolSuiteInterface * iObj)
{
   NLDRAAnalysis::CFunctionDeclaration functionDeclarationHolder;

   /* Load Function data for Source File from GLH API */
   iObj->loadFunctions(data->source_name, FileResults);

   /* Loop over each function in the file */
   while (iObj->readFunctionList(functionDeclarationHolder))
   {
      if (strcmp(functionDeclarationHolder.sourceFile().getLanguage(), "C") == 0)
      {
         int conforms = 1;
         int functionLineNumber = functionDeclarationHolder.getRefStartLine();
         char expectedName[MAXSTR] = {0};

         strcpy (expectedName, functionDeclarationHolder.getDeclarativeStem ());
         /* And finally add the _ */
         strcat (expectedName, "_");

         if (!functionDeclarationHolder.isStaticLinkage ()
             &&
             !functionDeclarationHolder.isMain ())
         {
            /* This is a global function so check that it conforms to <file>_<name> */
            conforms = !strncmp (expectedName, functionDeclarationHolder.getName(), strlen(expectedName) );
            /* Record the Result */
            process_naming_conformance (data, fp, "Global Function", "<file>_<name>", functionDeclarationHolder.getName(), conforms, functionDeclarationHolder.getRefStartLine(), 8);
         }
      }
  }
} /* End of check_global_functions */

/***----------------------------------------------------------------
***
*** Routine: check_file_size
*** This is an example of checking for a user-defined standard:
***
*** 1 Z File exceeds 30000 bytes in size
***
*** Parameters:
*** ===========
***
*** Action  Name                      Type            Use
*** ------  ----                      ----            ---
*** I       data                      const struct input_data *
*** I       fp                        FILE *
***
***
*** Return value:
*** void
***
***----------------------------------------------------------------*/
void check_file_size (const struct input_data* data, FILE* fp)
{
   char message[MESSAGE_SIZE] = {0};
   long src_size;
   const long PEN_1Z_LIMIT = 30000L;

   src_size = fsize (data->source_name);

   if (src_size > PEN_1Z_LIMIT)
   {
      sprintf (message,"File exceeds %d bytes in size : %s (%ld)",
         PEN_1Z_LIMIT, data->source_name, src_size);

      append_violation(data, fp, 1, message, 0, ZSTANDARD);
   }
} /* End of check_file_size */



/***----------------------------------------------------------------
***
*** Routine: check_filename_size
***
*** 2 Z Filename exceeds 8 characters
***  8 characters refers to the number of characters before the "."
***  (Currently assumes that extension is .c or .h)
***
*** Parameters:
*** ===========
***
*** Action  Name                      Type            Use
*** ------  ----                      ----            ---
*** I       data                      const struct input_data *
*** I       fp                        FILE *
***
***
*** Return value:
*** void
***
***
*** Calls:
*** ======
***
*** Name                      Location
*** ----                      --------
*** split_directory_path     This File
*** append_violation          This File
*** strlen                    System
*** sprintf                   System
***
***
***----------------------------------------------------------------*/
void check_filename_size (const struct input_data* data, FILE* fp)
{
   const int NAME_LIMIT = 8; /* excludes .c or .h */
   char short_name[MAXSTR] = {'\0'};
   char message[MESSAGE_SIZE] = {0};

   split_directory_path (data->source_name, short_name, (char*) NULL);

   if (strlen(short_name) > (NAME_LIMIT+2))  /* includes .c or .h */
   {
      sprintf (message,"Filename exceeds %d characters : %s (%d)",
               NAME_LIMIT, short_name, strlen(short_name));

      append_violation(data, fp, 2, message, 0, ZSTANDARD);
   }
} /* End of check_filename_size */



/***----------------------------------------------------------------
***
*** Routine: check_includename
***
*** 3 Z Include name does not match source file name.
***  Checks that the  #include filaname is the same as the source filename
***
*** Parameters:
*** ===========
***
*** Action  Name                Type            Use
*** ------  ----                ----            ---
*** I       data                const struct input_data *
*** I       fp                  FILE *
*** I       line                char *      line of source text
***
***
*** Return value:
*** void
***
***
*** Calls:
*** ======
***
*** Name                      Location
*** ----                      --------
*** split_directory_path     This File
*** append_violation          This File
*** strtok                    System
*** strcmp                    System
*** sprintf                   System
***
***
***----------------------------------------------------------------*/
void check_includename (const struct input_data* data, FILE* fp, char * line1)
{
   char message[MESSAGE_SIZE] = {0};
   char line[MAXSTR] = {0};
   char *include_tok;

   strcpy(line, line1);

   include_tok = strtok(line, " ");
   if ( (include_tok != NULL) && (!strcmp(include_tok, "#include")) )
   {
     char short_name[MAXSTR] = {'\0'};
     char c_source[MAXSTR] = {'\0'};
     char h_source[MAXSTR] = {'\0'};
     char *source_name;
     char *include_name;

     include_name = strtok(NULL, " <\""); /*excludes "" or < > */
     strcpy(h_source, include_name);
     include_name = strtok(include_name, ".");

     split_directory_path (data->source_name, short_name, (char*) NULL);
     strcpy(c_source, short_name);
     source_name = strtok(short_name, ".");


     if (strcmp(source_name, include_name))
     {
       sprintf(message,"Include name does not match source file name : %s %s",
               c_source, h_source);
       append_violation(data, fp, 3, message, 0, ZSTANDARD);
     }
   }
} /* End of check_includename */





/***----------------------------------------------------------------
***
*** Routine: match_template
***
*** Returns the index value of a template if found
*** Other wise returns -1
***
*** Parameters:
*** ===========
***
*** Action  Name                Type            Use
*** ------  ----                ----            ---
*** I       line                char *          line of source text
*** I       current_template    int             position in template list
***
*** Return value:
*** int
***
***
*** Calls:
*** ======
***
*** Name                      Location
*** ----                      --------
*** strncmp                   System
*** strlen                    System
***
***----------------------------------------------------------------*/
#define TEMPLATE_MAX 11
static char * usertemplate[TEMPLATE_MAX] =
{/* 0 */ "/*******************************************",
 /* 1 */ " * Copyright statement",
 /* 2 */ " *******************************************/",
 /* 3 */ "/*******************************************",
 /* 4 */ " * Filename:",
 /* 5 */ " * Author:",
 /* 6 */ " * Date:",
 /* 7 */ " * Purpose:",
 /* 8 */ " * Revision:",
 /* 9 */ " *******************************************/",
 /* 10 */ " /* Include Section */"
};

int match_template(char *line, int current_template)
{
  int template_no;
  for (template_no = current_template+1; template_no < TEMPLATE_MAX; template_no++)
  {
    /* Check to see whether template matches start of line */
    if ( !strncmp(usertemplate[template_no], line, strlen(usertemplate[template_no])) )
    {
      break;
    }
  }
  return  ( template_no == TEMPLATE_MAX ? -1 : template_no ) ;
} /* End of match_template */

/***----------------------------------------------------------------
***
*** Routine: check_templates
***
*** 4 Z Source does not match layout template.
***  Check to see whether any templates have been skipped.
***
*** Parameters:
*** ===========
***
*** Action  Name                Type            Use
*** ------  ----                ----            ---
*** I       data                const struct input_data *
*** I       fp                  FILE *
*** I       line                char *    current source line
***
*** Return value:
*** void
***
***
*** Calls:
*** ======
***
*** Name                      Location
*** ----                      --------
*** append_violation          This File
*** match_template            This File
*** sprintf                   System
***
***
***----------------------------------------------------------------*/
void check_templates (const struct input_data* data, FILE* fp, char * line)
{
   char message[MESSAGE_SIZE] = {0};
   static int current_template = -1;

   if ( current_template+1 < TEMPLATE_MAX )
   { /* In template section */
     int template_no = match_template(line, current_template);

     if ( template_no != -1 )
     {  /* template match found */
       if ( template_no != current_template+1 )
       {
           sprintf (message, "Source does not match layout template : %s",
                            usertemplate[current_template+1]);
           append_violation(data, fp, 4, message, 0, ZSTANDARD);
       }
       current_template = template_no;
     }
   }
} /* End of check_templates */



/***----------------------------------------------------------------
***
*** Routine: check_radix
***
*** 5 Z Hexadecimal number found.
***  Checks that  there is no use of hexadecimal radix
***
*** Parameters:
*** ===========
***
*** Action  Name                Type            Use
*** ------  ----                ----            ---
*** I       data                const struct input_data *
*** I       fp                  FILE *
*** I       line                char *          line of source text
***
***
*** Return value:
*** void
***
***
*** Calls:
*** ======
***
*** Name                      Location
*** ----                      --------
*** append_violation          This File
*** strstr                    System
*** sprintf                   System
***
***
***----------------------------------------------------------------*/
void check_radix (const struct input_data* data, FILE* fp, char * line)
{
   char message[MESSAGE_SIZE] = {0};
   char *s = strstr(line, "0x");
   if (s == NULL)
   {
     s = strstr(line, "0X");
   }
   if (s != NULL)
   {
     sprintf(message,"Hexadecimal number found : %s", s );
     append_violation(data, fp, 5, message, 5, ZSTANDARD);
   }
} /* End of check_radix */

/***----------------------------------------------------------------
***
*** Routine: check_forbidden_words
***
*** 6 Z Forbidden word found.
***  Checks that there is no use of forbidden words
***
*** Parameters:
*** ===========
***
*** Action  Name                Type            Use
*** ------  ----                ----            ---
*** I       data                const struct input_data *
*** I       fp                  FILE *
*** I       line                char *          line of source text
*** I       linenum             int             line number of text
***
***
*** Return value:
*** void
***
***
*** Calls:
*** ======
***
*** Name                      Location
*** ----                      --------
*** append_violation          This File
*** strstr                    System
*** sprintf                   System
***
***----------------------------------------------------------------*/
void check_forbidden_words (const struct input_data* data, FILE* fp, char * line, int linenum)
{
   char message[MESSAGE_SIZE] = {0};
   const int WORDS = 3;
   const char WORD_LIST[WORDS][32] = {"TODO", "TEMPORARY", "REMOVE" };

   for ( int i = 0; i < WORDS; i++ )
   {
      char *s = strstr(line, WORD_LIST[i]);
      if (s != NULL)
      {
         sprintf(message,"Forbidden word found : %s", WORD_LIST[i] );

         /*
          * Line number at this point is the actual source line number, before
          * storing the line number, we need to convert it to the reformatted code
          * line number.
          */
         int ref_linenum = linenum;
         tblinemapper_get_ref_line_num(linenum,
                                       data->source_name,
                                       data->module_id,
                                       data->source_name,
                                       data->workdir,
                                       &ref_linenum);

         append_violation(data, fp, 6, message, ref_linenum, ZSTANDARD);
      }
   }
} /* End of check_forbidden_words */

/***----------------------------------------------------------------
***
*** End of Standards
***
***----------------------------------------------------------------*/






/***----------------------------------------------------------------
***
*** Start of LDRA Functions to write Violation **Editing below this point is not recommended.
***
***----------------------------------------------------------------*/

/***-------------------------------------------------------------------------------------------------------------
***
*** Routine: process_naming_conformance
***
*** Parameters:
*** ===========
***
*** Action  Name            Type                          Use
*** ------  ----            ----                          ---
*** I       data            const struct input_data *     Argument Data
*** I       fp              FILE *                        SIF File Pointer
*** I       type            const char *                  Variable Type
*** I       style           const char *                  Conformance Style
*** I       name            const char *                  Variable Name
*** I       conforms        int                           Whether or not variable conforms to style
*** I       linenumber      int                           Line number of variable declaration
*** I       std_num         int                           Number for H Standard
***
*** Description:
*** ===========
*** If a variable does not conform to standards, adds it to the violation SIF file.
*** Also adds an entry to the naming report if reports are enabled
***
***-------------------------------------------------------------------------------------------------------------*/
void process_naming_conformance (const struct input_data * data,
                                  FILE * fp,
                                  const char* type,
                                  const char* style,
                                  const char* name,
                                  const int conforms,
                                  const int linenumber,
                                  const int std_num)
{
   char message[MAXSTR] = {0};

   /* If variable does not conform to standard, add it to the violations SIF file */
   if(!conforms)
   {
      append_violation (data, fp, std_num, name, linenumber, HSTANDARD);
#ifdef REPORT
      sprintf(message, "%s does not conform to style '%s': %s", type, style, name);
      print_to_report (message, linenumber, data);
#endif
   }
#ifdef REPORT
   else
   {
      sprintf(message, "%s conforms to style '%s': %s", type, style, name);
      print_to_report (message, linenumber, data);
   }
#endif
}

/***----------------------------------------------------------------
***
*** Routine: append_violation
***
*** Parameters:
*** ===========
***
*** Action  Name                 Type              Use
*** ------  ----                 ----              ---
*** I       data        const struct input_data *  source file information
*** I       fp                   FILE *            sif file
*** I       std_num              int               5th column in pen file
*** I       message              char *            message to be added to sif file
***
*** Description:
*** ============
***
*** Append a violation to the sif file (fp)
***
***----------------------------------------------------------------*/
void append_violation (const struct input_data* data,
                       FILE* fp,
                       const int std_num,
                       const char *message,
                       const int linenumber,
                       const char std_letter)
{
   struct sif_101_info std = {0,0,0,0,0,""};

   std.penfile_index = 0; /* penfile_index redundant (always 0) */
   std.procedure_number = 0;
   if (linenumber > 0)
   {
      std.ref_code_line_number = linenumber;
   }

   sprintf(std.standard_text, message);

   std.user_standard_number = std_num; /* This number must match column 5 in <lang>pen.dat */

   std.module_id_101 = data->module_id;

   print_101_struct (fp, &std, std_letter);
}/* End of append_violation */

/***----------------------------------------------------------------
***
*** Routine: get_sif_sub_file_name
***
*** Parameters:
*** ===========
***
*** Action  Name                      Type                              Use
*** ------  ----                      ----                              ---
*** I       data                      const struct input_data *         Argument Data
*** I       phase_id                  Phase_e                           Analysis Phase Enum
*** O       sfn_name                  char *                            Filename for SIF File
***
*** Description:
*** ============
***
***
***----------------------------------------------------------------*/
void get_sif_sub_file_name (const struct input_data* data,
                            Phase_e phase_id,
                            char* sfn_name)
{
   char short_name[MAXSTR] = {'\0'};
   char* nptr;
   char* extension = "";

   split_directory_path (data->source_name, short_name, (char*) NULL);
/*
 * Remove the language extension
 */
   nptr = strrchr (short_name, '.');
   if (nptr != NULL)
   {
     *nptr = '\0';
   }

/*
 * Determine the extension
 */
   switch (phase_id)
   {
      case Msa:
        extension = "sf1";
        break;
      case Cmp:
        extension = "sf2";
        break;
      case Dfl:
        extension = "sf3";
        break;
      case Xrf:
        extension = "sf6";
        break;
      case Inf:
        extension = "sf5";
        break;
      default:
      /* Default is sf1 */
        extension = "sf1";
        break;
   }

   sprintf (sfn_name, "%s\\%s_%d.%s", data->workdir, short_name,
                                      data->module_id,
                                      extension);
} /* End of get_sif_sub_file_name */

/***----------------------------------------------------------------
***
*** Routine: print_101_struct
*** Wrapper for fprintf to print 101 record in correct format.
***
*** Parameters:
*** Action  Name                      Type                      Use
*** ------  ----                      ----                      ---
*** I       fp                        FILE *                    SIF File Pointer
*** I       std                       struct sif_101_info *     Data to add to SIF File
***
*** Return value:
*** void
***
*** Calls:
*** Name                      Location
*** ----                      --------
*** fprintf                   System
*** strlen                    System
***
***----------------------------------------------------------------*/
void print_101_struct (FILE* fp, const struct sif_101_info* std, const char std_letter)
{
   fprintf(fp,
      "%4d %3d 101 000000 0000 %6d %6d %6d %6d %c %2d %s\n",
      std->module_id_101,
      std->procedure_number,
      std->ref_code_line_number,
      std->penfile_index,
      1,
      std->user_standard_number,
      std_letter,
      strlen (std->standard_text),
      std->standard_text);
} /* End of print_101_struct */


/***----------------------------------------------------------------
***
*** Routine: split_directory_path
*** Splits file_name parameter into directory_name and reduced_name
***
*** Parameters:
***
*** Action  Name                      Type              Use
*** ------  ----                      ----              ---
*** I       file_name                 const char *      Original file name including path
*** IO      reduced_name              char *            Filename without path
*** IO      directory_name            char *            Path without filename
***
*** Return value:
*** void
***
*** Calls:
***
*** Name                      Location
*** ----                      --------
*** strcpy                    System
*** strlen                    System
*** strncpy                   System
***----------------------------------------------------------------*/
void split_directory_path (const char *file_name,
                            char *reduced_name,
                            char *directory_name)
 {
   size_t lstrt,lend;
   char temp_string[MAXSTR];

   lend = strlen(file_name);
   lstrt = lend;

   while ((lstrt > 0) &&
      ((file_name[lstrt] != '\\') &&
      (file_name[lstrt] != ':') &&
      (file_name[lstrt] != '/')))
   {
      lstrt--;
   }
   if (lstrt != 0)
   {
      lstrt++;
   }
   if (directory_name != NULL)
   {
      strncpy (directory_name,file_name,lstrt);
      directory_name[lstrt] = '\0';
   }
   if (reduced_name != NULL)
   {
      strncpy (temp_string, &file_name[lstrt], lend - lstrt);
      temp_string[lend-lstrt] = '\0';
      strcpy (reduced_name,temp_string);
   }
 } /* end of split_directory_path */

/***----------------------------------------------------------------
***
*** End of LDRA Functions to write Violation
***
***----------------------------------------------------------------*/





/***----------------------------------------------------------------
***
*** Start of LDRA Functions to write .naming Report
***
***----------------------------------------------------------------*/
#ifdef REPORT

FILE* fpRep = NULL;

/***-------------------------------------------------------------------------------------------------------------
***
*** Routine: print_to_report
***
*** Parameters:
*** ===========
***
*** Action  Name            Type                          Use
*** ------  ----            ----                          ---
*** I       message         const char *                  Message to write in report
*** I       linenumber      int                           Line Number of variable declaration
*** I       data            const struct input_data *     Argument Data
***
*** Description:
*** ===========
*** Writes the given message to the naming report file
***
***-------------------------------------------------------------------------------------------------------------*/
void  print_to_report (const char* message, const int linenumber, const struct input_data* data)
{
   if (fpRep == NULL)
   {
      char rep_name[MAXSTR];
      char short_name[MAXSTR] = {'\0'};
      char* nptr;
      char* extension = "";
      split_directory_path (data->source_name, short_name, (char*) NULL);
      nptr = strrchr (short_name, '.');
      if (nptr != NULL)
      {
         *nptr = '\0';
      }
      sprintf (rep_name, "%s\\%s_%d.naming", data->workdir,
                                             short_name,
                                             data->module_id);

      fpRep = fopen ( rep_name, "w");
   }
   if (fpRep != NULL)
   {
      fprintf (fpRep, "%-6d %s\n", linenumber, message);
   }
}

/***-------------------------------------------------------------------------------------------------------------
***
*** Routine: close_report
***
*** Description:
*** ===========
*** Closes the file pointer for the report
***
***-------------------------------------------------------------------------------------------------------------*/
void close_report ()
{
   if (fpRep != NULL)
   {
      fclose (fpRep);
   }
}
#endif

/***----------------------------------------------------------------
***
*** End of LDRA Functions to write .naming Report
***
***----------------------------------------------------------------*/



/***----------------------------------------------------------------
***
*** Routine: fsize
*** Uses standard C library routines to determine size in bytes of
*** a file.
***
*** Parameters:
*** Action  Name                      Type            Use
*** ------  ----                      ----            ---
*** I       infile                    const char *    Name of file
***
***
*** Return value:
*** Type            Meaning
*** ----            -------
*** long           Size in bytes of the input file, 0 if
***                file not opened
*** Calls:
*** Name                      Location
*** ----                      --------
*** fclose                    System
*** fopen                     System
*** fseek                     System
*** ftell                     System
***----------------------------------------------------------------*/
long fsize (const char *infile)
 {
   FILE *fd ;
   long size = 0L;
   fd = fopen(infile,"r") ;
   if (fd != NULL)
   {
      fseek(fd,0L,2) ;
      size = ftell(fd) ;
      fclose(fd) ;
   }
   return(size) ;
 } /* end of fsize */
