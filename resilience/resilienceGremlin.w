//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013, Lawrence Livermore National Security, LLC.
// Produced at the Lawrence Livermore National Laboratory
//
// Written by Martin Schulz et al <schulzm@llnl.gov>
// LLNL-CODE-645436
// All rights reserved.
//
// This file is part of the GREMLINS framework.
// For details, see http://scalability.llnl.gov/gremlins.
// Please read the LICENSE file for further information.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the disclaimer below.
//
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the disclaimer (as
//       noted below) in the documentation and/or other materials
//       provided with the distribution.
//
//     * Neither the name of the LLNS/LLNL nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE
// LIVERMORE NATIONAL SECURITY, LLC, THE U.S. DEPARTMENT OF ENERGY OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//////////////////////////////////////////////////////////////////////////////

#include "fault_injector.h"

{{fn foo MPI_Init}}
        {{callfn}}
       	GREMLIN_initExperiment(); 
	PMPI_Barrier(MPI_COMM_WORLD);
{{endfn}}


{{fn foo MPI_Finalize}}
        PMPI_Barrier(MPI_COMM_WORLD);
	GREMLIN_endExperiment();	
        {{callfn}}
{{endfn}}
