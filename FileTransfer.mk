##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=FileTransfer
ConfigurationName      :=Debug
WorkspacePath          :=/Users/ericwei/test/Network
ProjectPath            :=/Users/ericwei/test/Network
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Eric Wei
Date                   :=25/05/2019
CodeLitePath           :="/Users/ericwei/Library/Application Support/CodeLite"
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -dynamiclib -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="FileTransfer.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -g -O0 -Wall -std=c++11 $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/Users/ericwei/codelite.app/Contents/SharedSupport/
Objects0=$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/Server.cpp$(ObjectSuffix) $(IntermediateDirectory)/Client.cpp$(ObjectSuffix) $(IntermediateDirectory)/ClientSession.cpp$(ObjectSuffix) $(IntermediateDirectory)/Socket.cpp$(ObjectSuffix) $(IntermediateDirectory)/TCPSocket.cpp$(ObjectSuffix) $(IntermediateDirectory)/UDPSocket.cpp$(ObjectSuffix) $(IntermediateDirectory)/UnitTest.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/ericwei/test/Network/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp

$(IntermediateDirectory)/Server.cpp$(ObjectSuffix): Server.cpp $(IntermediateDirectory)/Server.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/ericwei/test/Network/Server.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Server.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Server.cpp$(DependSuffix): Server.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Server.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Server.cpp$(DependSuffix) -MM Server.cpp

$(IntermediateDirectory)/Server.cpp$(PreprocessSuffix): Server.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Server.cpp$(PreprocessSuffix) Server.cpp

$(IntermediateDirectory)/Client.cpp$(ObjectSuffix): Client.cpp $(IntermediateDirectory)/Client.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/ericwei/test/Network/Client.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Client.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Client.cpp$(DependSuffix): Client.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Client.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Client.cpp$(DependSuffix) -MM Client.cpp

$(IntermediateDirectory)/Client.cpp$(PreprocessSuffix): Client.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Client.cpp$(PreprocessSuffix) Client.cpp

$(IntermediateDirectory)/ClientSession.cpp$(ObjectSuffix): ClientSession.cpp $(IntermediateDirectory)/ClientSession.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/ericwei/test/Network/ClientSession.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ClientSession.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ClientSession.cpp$(DependSuffix): ClientSession.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ClientSession.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ClientSession.cpp$(DependSuffix) -MM ClientSession.cpp

$(IntermediateDirectory)/ClientSession.cpp$(PreprocessSuffix): ClientSession.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ClientSession.cpp$(PreprocessSuffix) ClientSession.cpp

$(IntermediateDirectory)/Socket.cpp$(ObjectSuffix): Socket.cpp $(IntermediateDirectory)/Socket.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/ericwei/test/Network/Socket.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Socket.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Socket.cpp$(DependSuffix): Socket.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Socket.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Socket.cpp$(DependSuffix) -MM Socket.cpp

$(IntermediateDirectory)/Socket.cpp$(PreprocessSuffix): Socket.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Socket.cpp$(PreprocessSuffix) Socket.cpp

$(IntermediateDirectory)/TCPSocket.cpp$(ObjectSuffix): TCPSocket.cpp $(IntermediateDirectory)/TCPSocket.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/ericwei/test/Network/TCPSocket.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/TCPSocket.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/TCPSocket.cpp$(DependSuffix): TCPSocket.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/TCPSocket.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/TCPSocket.cpp$(DependSuffix) -MM TCPSocket.cpp

$(IntermediateDirectory)/TCPSocket.cpp$(PreprocessSuffix): TCPSocket.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/TCPSocket.cpp$(PreprocessSuffix) TCPSocket.cpp

$(IntermediateDirectory)/UDPSocket.cpp$(ObjectSuffix): UDPSocket.cpp $(IntermediateDirectory)/UDPSocket.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/ericwei/test/Network/UDPSocket.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/UDPSocket.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/UDPSocket.cpp$(DependSuffix): UDPSocket.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/UDPSocket.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/UDPSocket.cpp$(DependSuffix) -MM UDPSocket.cpp

$(IntermediateDirectory)/UDPSocket.cpp$(PreprocessSuffix): UDPSocket.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/UDPSocket.cpp$(PreprocessSuffix) UDPSocket.cpp

$(IntermediateDirectory)/UnitTest.cpp$(ObjectSuffix): UnitTest.cpp $(IntermediateDirectory)/UnitTest.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/ericwei/test/Network/UnitTest.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/UnitTest.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/UnitTest.cpp$(DependSuffix): UnitTest.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/UnitTest.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/UnitTest.cpp$(DependSuffix) -MM UnitTest.cpp

$(IntermediateDirectory)/UnitTest.cpp$(PreprocessSuffix): UnitTest.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/UnitTest.cpp$(PreprocessSuffix) UnitTest.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


