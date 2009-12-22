/*
Planarity-Related Graph Algorithms Project
Copyright (c) 1997-2009, John M. Boyer
All rights reserved. Includes a reference implementation of the following:

* John M. Boyer. "Simplified O(n) Algorithms for Planar Graph Embedding,
  Kuratowski Subgraph Isolation, and Related Problems". Ph.D. Dissertation,
  University of Victoria, 2001.

* John M. Boyer and Wendy J. Myrvold. "On the Cutting Edge: Simplified O(n)
  Planarity by Edge Addition". Journal of Graph Algorithms and Applications,
  Vol. 8, No. 3, pp. 241-273, 2004.

* John M. Boyer. "A New Method for Efficiently Generating Planar Graph
  Visibility Representations". In P. Eades and P. Healy, editors,
  Proceedings of the 13th International Conference on Graph Drawing 2005,
  Lecture Notes Comput. Sci., Volume 3843, pp. 508-511, Springer-Verlag, 2006.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* Neither the name of the Planarity-Related Graph Algorithms Project nor the names
  of its contributors may be used to endorse or promote products derived from this
  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "planarity.h"

/****************************************************************************
 SpecificGraph()
 ****************************************************************************/

int SpecificGraph(char command, char *infileName, char *outfileName, char *outfile2Name)
{
graphP theGraph, origGraph;
platform_time start, end;
int Result;

    // Get the filename of the graph to test
    if ((infileName = ConstructInputFilename(infileName)) == NULL)
	    return NOTOK;

    // Create the graph and attach the correct algorithm to it
    theGraph = gp_New();
    AttachAlgorithm(theGraph, command);

    // Read the graph into memory
	Result = gp_Read(theGraph, infileName);
	if (Result == NONEMBEDDABLE)
	{
		Message("The graph contains too many edges.\n");
		// Some of the algorithms will still run correctly with some edges removed.
		if (strchr("pdo234", command))
		{
			Message("Some edges were removed, but the algorithm will still run correctly.\n");
			Result = OK;
		}
	}

	// If there was an unrecoverable error, report it
	if (Result != OK)
		ErrorMessage("Failed to read graph\n");

	// Otherwise, call the correct algorithm on it
	else
	{
        origGraph = gp_DupGraph(theGraph);

    	switch (command)
    	{
    		case 'p' :
    	        platform_GetTime(start);
    			Result = gp_Embed(theGraph, EMBEDFLAGS_PLANAR);
    	        platform_GetTime(end);
    	        sprintf(Line, "The graph is%s planar.\n", Result==OK ? "" : " not");
    	        Result = gp_TestEmbedResultIntegrity(theGraph, origGraph, Result);
    			break;
    		case 'd' :
    	        platform_GetTime(start);
    			Result = gp_Embed(theGraph, EMBEDFLAGS_DRAWPLANAR);
    	        platform_GetTime(end);
    	        sprintf(Line, "The graph is%s planar.\n", Result==OK ? "" : " not");
    	        Result = gp_TestEmbedResultIntegrity(theGraph, origGraph, Result);
    			break;
    		case 'o' :
    	        platform_GetTime(start);
    			Result = gp_Embed(theGraph, EMBEDFLAGS_OUTERPLANAR);
    	        platform_GetTime(end);
    	        sprintf(Line, "The graph is%s outerplanar.\n", Result==OK ? "" : " not");
    	        Result = gp_TestEmbedResultIntegrity(theGraph, origGraph, Result);
    			break;
    		case '2' :
    	        platform_GetTime(start);
    			Result = gp_Embed(theGraph, EMBEDFLAGS_SEARCHFORK23);
    	        platform_GetTime(end);
    	        sprintf(Line, "The graph %s a subgraph homeomorphic to K_{2,3}.\n", Result==OK ? "does not contain" : "contains");
    	        Result = gp_TestEmbedResultIntegrity(theGraph, origGraph, Result);
    			break;
    		case '3' :
    	        platform_GetTime(start);
				Result = gp_Embed(theGraph, EMBEDFLAGS_SEARCHFORK33);
		        platform_GetTime(end);
    	        sprintf(Line, "The graph %s a subgraph homeomorphic to K_{3,3}.\n", Result==OK ? "does not contain" : "contains");
    	        Result = gp_TestEmbedResultIntegrity(theGraph, origGraph, Result);
				break;
    		case '4' :
    	        platform_GetTime(start);
    			Result = gp_Embed(theGraph, EMBEDFLAGS_SEARCHFORK4);
    	        platform_GetTime(end);
    	        sprintf(Line, "The graph %s a subgraph homeomorphic to K_4.\n", Result==OK ? "does not contain" : "contains");
    	        Result = gp_TestEmbedResultIntegrity(theGraph, origGraph, Result);
    			break;
    		case 'c' :
    	        platform_GetTime(start);
    			Result = gp_ColorVertices(theGraph);
    	        platform_GetTime(end);
    	        sprintf(Line, "The graph has been %d-colored.\n", gp_GetNumColorsUsed(theGraph));
    	        Result = gp_ColorVerticesIntegrityCheck(theGraph, origGraph);
    			break;
    		default :
    	        platform_GetTime(start);
    			Result = NOTOK;
    	        platform_GetTime(end);
    	        sprintf(Line, "Unrecognized Command\n");
    			break;
    	}

    	// Show the result message for the algorithm
        Message(Line);

    	// Report the length of time it took
        sprintf(Line, "Algorithm '%s' executed in %.3lf seconds.\n",
        		GetAlgorithmName(command), platform_GetDuration(start,end));
        Message(Line);

        // Free the graph obtained for integrity checking.
        gp_Free(&origGraph);
	}

	// Report an error, if there was one, free the graph, and return
	if (Result != OK && Result != NONEMBEDDABLE)
	{
		ErrorMessage("AN ERROR HAS BEEN DETECTED\n");
		Result = NOTOK;
	}

	// Provide the output file(s)
	else
	{
        // Restore the vertex ordering of the original graph (undo DFS numbering)
        gp_SortVertices(theGraph);

        // Determine the name of the primary output file
        outfileName = ConstructPrimaryOutputFilename(infileName, outfileName, command);

        // For some algorithms, the primary output file is not always written
        if ((strchr("pdo", command) && Result == NONEMBEDDABLE) ||
        	(strchr("234", command) && Result == OK))
        	;

        // Write the primary output file, if appropriate to do so
        else
			gp_Write(theGraph, outfileName, WRITE_ADJLIST);

        // NOW WE WANT TO WRITE THE SECONDARY OUTPUT FILE

		// When called from the menu system, we want to write the planar or outerplanar
		// obstruction, if one exists. For planar graph drawing, we want the character
        // art rendition.  A non-NULL empty string is passed to indicate the necessity
        // of selecting a default name for the second output file.
		if (outfile2Name != NULL)
		{
			if ((command == 'p' || command == 'o') && Result == NONEMBEDDABLE)
			{
				if (strlen(outfile2Name) == 0)
				    outfile2Name = outfileName;
				gp_Write(theGraph, outfile2Name, WRITE_ADJLIST);
			}
			else if (command == 'd' && Result == OK)
			{
				if (strlen(outfile2Name) == 0)
   				    strcat((outfile2Name = outfileName), ".render.txt");
				gp_DrawPlanar_RenderToFile(theGraph, outfile2Name);
			}
		}
	}

	// Free the graph and return the result
	gp_Free(&theGraph);
	return Result;
}
