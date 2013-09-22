// **************************************************************************
//     Filename: MCFunctions.CPP
// Date Created: 9/21/2013
//      Version: 0.01
//
//
//  COPYRIGHT (c) Robertsonics 2013. All Rights Reserved
//
// **************************************************************************
//
// Revision History
//
// Date/Rev  Description
// --------  -----------
//
// V0.01     First version created.
// 9/21/13


// **************************************************************************
// resetTrigger(): Restore the Trigger settings to default
void MainComponent::resetTrigger(void)
{
	interfaceBox->setSelectedId(1);
	functionBox->setSelectedId(1);
	typeBox->setSelectedId(1);
	invertToggle->setToggleState(false, dontSendNotification);
	retriggerToggle->setToggleState(true, dontSendNotification);
	polyToggle->setToggleState(false, dontSendNotification);
	lowText->setText("", dontSendNotification);
	highText->setText("", dontSendNotification);
}

// **************************************************************************
// reset(): Reset everything to default, clear the edit window.
void MainComponent::reset(void)
{
	sampleRateBox->setSelectedId(3);
	baudBox->setSelectedId(4);
	volSlider->setValue(0.0, dontSendNotification);
	triggerBox->setSelectedId(1);
	resetTrigger();

	addButton->setEnabled(true);
	updateButton->setEnabled(false);
	deleteButton->setEnabled(false);

	mInitStrings.clear();
	mInitStrings.add("********************************************************************");
	mInitStrings.add(newLine);
	mInitStrings.add("This file was generated by the WAV Trigger Pro Init File Utility.");
	mInitStrings.add(newLine);
	mInitStrings.add("Entries are only required if you wish to over-ride default settings.");
	mInitStrings.add(newLine);
	mInitStrings.add("You can add your own comments below this line -->");
	updateInitWindow();
}

// **************************************************************************
// buildTriggerDesc(): Create a #TRIG NN line according to the current
//  settings
String MainComponent::buildTriggerDesc(void)
{

String tStr;

	// Trigger command and trigger number
	tStr = "#TRIG ";
	if (triggerBox->getSelectedId() < 10)
		tStr += 0;
	tStr += triggerBox->getSelectedId();
	tStr += ", ";

	// Hardware interface
	tStr += interfaceBox->getSelectedId();
	tStr += ", ";

	// Invert Flag
	tStr += invertToggle->getToggleState();
	tStr += ", ";

	// Type
	tStr += typeBox->getSelectedId();
	tStr += ", ";

	// Retrigger Flag
	tStr += retriggerToggle->getToggleState();
	tStr += ", ";

	// Polyphonic Flag
	tStr += polyToggle->getToggleState();
	tStr += ", ";

	// Function
	tStr += functionBox->getSelectedId();
	tStr += ", ";

	// Low range
	tStr += lowText->getText();
	tStr += ", ";

	// High range
	tStr += highText->getText();

	tStr += newLine;
	return tStr;
}

// **************************************************************************
// getTrigger(): Look for a trigger number in this string
int MainComponent::getTriggerNum(String tStr)
{

int t;
String subStr;

	if (!tStr.startsWith("#TRIG "))
		return -1;

	subStr = tStr.trimCharactersAtStart("#TRIG ");
	subStr = subStr.upToFirstOccurrenceOf(",", false, false);
	t = subStr.getIntValue();
	if ((t >= 1 || t <= 16))
		return t;

	return -1;
}


// **************************************************************************
// findTrigger(): Look for a trigger entry in the string array
int MainComponent::findTrigger(int t)
{
	for (int i = 0; i < mInitStrings.size(); i++) {
		if (getTriggerNum(mInitStrings[i]) == t)
			return i;
	}
	return -1;
}


// **************************************************************************
// insertTriggerString(): insert an entry for the current trigger settings
//  into the correct element in the init string array.
void MainComponent::insertTriggerString(void)
{

int n = 0;
bool found = false;

	while ((n < mInitStrings.size()) && !found) {
		if (getTriggerNum(mInitStrings[n]) > triggerBox->getSelectedId())
			found = true;
		else if (mInitStrings[n].contains("*"))
			found = true;
		else
			n++;
	}
	if (found)
		mInitStrings.insert(n, buildTriggerDesc());
	else
		mInitStrings.add(buildTriggerDesc());
		
}



// **************************************************************************
// parseTriggerDesc(): Parse the string and update the settings accordingly
bool MainComponent::parseTriggerDesc(String tStr)
{

int t, n, i;
String subStr;
StringArray tokens;

	if (!tStr.startsWith("#TRIG "))
		return false;

	// Parse the paramaters into a token string array
	subStr = tStr.trimCharactersAtStart("#TRIG ");
	tokens.addTokens(subStr, false);
	n = tokens.size();
	if (n == 0)
		return false;

	t = tokens[0].getIntValue();
	if ((t < 0) || (t > 16))
		return false;
	triggerBox->setSelectedId(t);
	resetTrigger();

	// Read and set the hardware interface type
	if (n > 1) {
		i = tokens[1].getIntValue();
		if ((i > 0) && (i <= interfaceBox->getNumItems()))
			interfaceBox->setSelectedId(i);
	}

	// Read and set the invert toggle
	if (n > 2) {
		i = tokens[2].getIntValue();
		if (i == 0)
			invertToggle->setToggleState(false, dontSendNotification);
		else
			invertToggle->setToggleState(true, dontSendNotification);
	}

	// Read and set the trigger type
	if (n > 3) {
		i = tokens[3].getIntValue();
		if ((i > 0) && (i <= typeBox->getNumItems()))
			typeBox->setSelectedId(i);
	}

	// Read and set the retrigger toggle
	if (n > 4) {
		i = tokens[4].getIntValue();
		if (i == 0)
			retriggerToggle->setToggleState(false, dontSendNotification);
		else
			retriggerToggle->setToggleState(true, dontSendNotification);
	}

	// Read and set the polyphonic toggle
	if (n > 5) {
		i = tokens[5].getIntValue();
		if (i == 0)
			polyToggle->setToggleState(false, dontSendNotification);
		else
			polyToggle->setToggleState(true, dontSendNotification);
	}

	// Read and set the trigger function
	if (n > 6) {
		i = tokens[6].getIntValue();
		if ((i > 0) && (i <= functionBox->getNumItems()))
			functionBox->setSelectedId(i);
	}

	// Read and fill in the track range
	if (n > 7)
		lowText->setText(tokens[7]);
	if (n > 8)
		highText->setText(tokens[8]);

	return true;

}

// **************************************************************************
// deleteInitString(): Look for a trigger entry in the string array
void MainComponent::deleteInitString(int n)
{
	if (n >= mInitStrings.size())
		return;
	mInitStrings.remove(n);

}



void MainComponent::updateInitWindow(void)
{

int i;
String initStr;

	initText->clear();
	for (i = 0; i < mInitStrings.size(); i++) {
		initStr.append(mInitStrings[i], 1024);
	}
	initText->setText(initStr);

}

void MainComponent::save(File saveFile)
{

String line;

	if (saveFile.exists())
		saveFile.deleteFile();
	if (saveFile.create()) {
		FileOutputStream * outStream = saveFile.createOutputStream();

		outStream->writeText(initText->getText(), 0, 0);

		outStream->writeText(newLine, 0, 0);
		outStream->writeText("**********************************************************************", 0, 0);
		outStream->writeText(newLine, 0, 0);
		outStream->writeText("This file was generated by the WAV Trigger Pro Init File Utility v0.10", 0, 0);
		outStream->writeText(newLine, 0, 0);
		delete outStream;
	}
}

