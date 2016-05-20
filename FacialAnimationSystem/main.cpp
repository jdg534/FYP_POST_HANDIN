#include <iostream>
#include <fstream>

#include <fmod.hpp>
#include <fmod_errors.h>



#include <Windows.h> // for SoundSystem("PAUSE");

#include "playBackManager.h"

#include "ErrorLog.h"
#include "Application.h"


#include "SoundData.h"

// 

// int main()

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevousInst,LPSTR lpCmdLine, int nCmdShow)
{
	/*PlayBackManager * pm = PlayBackManager::getInstance();

	if (!pm->isInitialised())
	{
		if (!pm->init())
		{
			pm->shutdown();
			delete pm;
			return -1;
		}
	}
	*/
	
	// all to go in playbackManager
	/*
	SoundData sd;

	sd.loadFromFile("Phonemes/a.wav");

	std::ofstream outTxt("a.txt");
	outTxt.clear();
	sd.writeAsText(outTxt);
	outTxt.close();
	*/
	
	// quick test output
	/*
	std::vector<std::string> audioFiles;
	audioFiles.push_back("ee.wav");
	audioFiles.push_back("eh.wav");
	audioFiles.push_back("a.wav");
	audioFiles.push_back("i.wav");
	audioFiles.push_back("er.wav");
	audioFiles.push_back("uAlt.wav");
	audioFiles.push_back("u.wav");
	audioFiles.push_back("err.wav");
	audioFiles.push_back("ar.wav");
	audioFiles.push_back("oo.wav");
	audioFiles.push_back("or.wav");
	audioFiles.push_back("o.wav");
	audioFiles.push_back("ea.wav");
	audioFiles.push_back("ere.wav");
	audioFiles.push_back("ou.wav");
	audioFiles.push_back("au.wav");
	audioFiles.push_back("ay.wav");
	audioFiles.push_back("iy.wav");
	audioFiles.push_back("oy.wav");
	audioFiles.push_back("p.wav");
	audioFiles.push_back("f.wav");
	audioFiles.push_back("t.wav");
	audioFiles.push_back("th.wav");
	audioFiles.push_back("ch.wav");
	audioFiles.push_back("s.wav");
	audioFiles.push_back("sh.wav");
	audioFiles.push_back("k.wav");
	audioFiles.push_back("b.wav");
	audioFiles.push_back("v.wav");
	audioFiles.push_back("d.wav");
	audioFiles.push_back("thv.wav");
	audioFiles.push_back("jg.wav");
	audioFiles.push_back("z.wav");
	audioFiles.push_back("gg.wav");
	audioFiles.push_back("g.wav");
	audioFiles.push_back("h.wav");
	audioFiles.push_back("m.wav");
	audioFiles.push_back("n.wav");
	audioFiles.push_back("nn.wav");
	audioFiles.push_back("r.wav");
	audioFiles.push_back("l.wav");
	audioFiles.push_back("w.wav");
	audioFiles.push_back("y.wav");
	for (int i = 0; i < audioFiles.size(); i++)
	{
		std::string dir = "Phonemes/";
		SoundData tmpSD;
		tmpSD.loadFromFile(dir + audioFiles[i]);

		std::string filePreExtention;
		int dotIndex = audioFiles[i].find_first_of(".");
		filePreExtention = audioFiles[i].substr(0, dotIndex);
		filePreExtention += ".txt";
		std::ofstream afOutTxt("PhonemeTextRepresentations/" + filePreExtention);
		afOutTxt.clear();
		tmpSD.writeAsText(afOutTxt);
		afOutTxt.close();
	}
	*/
	Application * app = new Application();

	ErrorLog * errLog = ErrorLog::getInstance();

	bool appInitialiseSuccesfully = app->init("appIndex.txt", hInst, nCmdShow);

	if (!appInitialiseSuccesfully)
	{
		app->shutdown();
		delete app;
		errLog->logMinorError("couldn't run Application.init()");
		errLog->shutdown();
		delete app;
		delete errLog;
		return -1;
	}

	app->run();

	app->shutdown();

	delete app;

	return 0;

}