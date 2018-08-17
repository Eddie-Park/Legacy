<distribution version="10.0.0" name="Genisys4" type="MSI">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{2FB6E069-EC30-4EBE-8C90-8C62633B077D}">
		<general appName="Genisys4" outputLocation="c:\Documents and Settings\Richard\My Documents\GENISYS4_GUI 1234\cvidistkit.Genisys4" relOutputLocation="cvidistkit.Genisys4" outputLocationWithVars="c:\Documents and Settings\Richard\My Documents\GENISYS4_GUI 1234\cvidistkit.%name" relOutputLocationWithVars="cvidistkit.%name" upgradeBehavior="1" autoIncrement="true" version="1.0.3">
			<arp company="Sofie Biosciences" companyURL="http://sofiebio.com" supportURL="" contact="Nam T. Vu" phone="(310) 242-6794" comments=""/>
			<summary title="" subject="" keyWords="" comments="" author=""/></general>
		<userinterface language="English" showPaths="true" readMe="" license="">
			<installerImages>
				<welcome>
					<path>c:\Documents and Settings\Richard\My Documents\GENISYS4_GUI 1234\GenisysInstall.bmp</path>
					<relPath>GenisysInstall.bmp</relPath></welcome></installerImages>
			<dlgstrings welcomeTitle="Genisys4" welcomeText=""/></userinterface>
		<dirs appDirID="101">
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="Genisys4" dirID="100" parentID="7" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="[Program Files]" dirID="2" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="Genisys4" dirID="101" parentID="2" isMSIDir="false" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="c:\Documents and Settings\Richard\My Documents\GENISYS4_GUI 1234\cvibuild.genisys4\Release\genisys4.exe" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="C:\Documents and Settings\Richard\My Documents\GENISYS4_GUI 1234\PGRFlyCapture.dll" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="2" sourcePath="c:\Documents and Settings\Richard\My Documents\GENISYS4_GUI 1234\histogram.exe" relSourcePath="histogram.exe" relSourceBase="0" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput targetType="0" dirID="101" projectID="0">
				<fileID>0</fileID></projectOutput>
			<projectDependencies dirID="101" projectID="0">
				<fileID>1</fileID></projectDependencies></fileGroups>
		<shortcuts>
			<shortcut name="Genisys4" targetFileID="0" destDirID="100" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
		<mergemodules/>
		<products/>
		<runtimeEngine cvirte="true" instrsup="true" lvrt="true" analysis="true" netvarsup="true" dotnetsup="true" activeXsup="true" lowlevelsup="true" rtutilsup="true" installToAppDir="false"/>
		<advanced mediaSize="650">
			<launchConditions>
				<condition>MINOS_WINXP</condition>
			</launchConditions>
			<includeConfigProducts>true</includeConfigProducts>
			<maxImportVisible>silent</maxImportVisible>
			<maxImportMode>merge</maxImportMode>
			<custMsgFlag>false</custMsgFlag>
			<custMsgPath>c:\program files\national instruments\cvi2010\bin\msgrte.txt</custMsgPath>
			<signExe>false</signExe>
			<certificate></certificate>
			<signTimeURL></signTimeURL>
			<signDescURL></signDescURL></advanced>
		<Projects NumProjects="1">
			<Project000 ProjectID="0" ProjectAbsolutePath="c:\Documents and Settings\Richard\My Documents\GENISYS4_GUI 1234\genisys4.prj" ProjectRelativePath="genisys4.prj"/></Projects>
		<buildData progressBarRate="0.757935711724291">
			<progressTimes>
				<Begin>0.000000000000000</Begin>
				<ProductsAdded>0.238128625000001</ProductsAdded>
				<DPConfigured>3.550891250000000</DPConfigured>
				<DPMergeModulesAdded>8.511246249999989</DPMergeModulesAdded>
				<DPClosed>13.346821749999993</DPClosed>
				<DistributionsCopied>17.781660999999978</DistributionsCopied>
				<End>131.937311375000010</End></progressTimes></buildData>
	</msi>
</distribution>
