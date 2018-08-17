<distribution version="10.0.0" name="Genisys4" type="MSI64">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{279B8D03-185E-4225-B4AE-7E511AA3E439}" x64="true">
		<general appName="Genisys4" outputLocation="c:\Users\rtaschereau\Documents\GENISYS4_GUI 1292\cvidistkit.Genisys4" relOutputLocation="cvidistkit.Genisys4" outputLocationWithVars="c:\Users\rtaschereau\Documents\GENISYS4_GUI 1292\cvidistkit.%name" relOutputLocationWithVars="cvidistkit.%name" upgradeBehavior="1" autoIncrement="true" version="1.0.26">
			<arp company="Sofie BioSciences Inc." companyURL="http://www.sofiebio.com/" supportURL="" contact="" phone="(310) 242-6794" comments=""/>
			<summary title="" subject="" keyWords="" comments="" author=""/></general>
		<userinterface language="English" showPaths="false" readMe="" license="">
			<installerImages>
				<welcome>
					<path>c:\Users\rtaschereau\Documents\GENISYS4_GUI 1292\GenisysInstall.bmp</path>
					<relPath>GenisysInstall.bmp</relPath></welcome></installerImages>
			<dlgstrings welcomeTitle="Genisys4" welcomeText=""/></userinterface>
		<dirs appDirID="100">
			<installDir name="[Program Files]" dirID="24" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="Genisys4" dirID="100" parentID="24" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="Genisys4" dirID="101" parentID="7" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="C:" dirID="102" parentID="-1" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="Users" dirID="103" parentID="102" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="Sofie" dirID="104" parentID="103" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="AppData" dirID="105" parentID="104" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="Roaming" dirID="106" parentID="105" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="PETBox" dirID="107" parentID="106" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="NormalizationFiles" dirID="108" parentID="107" isMSIDir="false" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="c:\Users\rtaschereau\Documents\GENISYS4_GUI 1292\Recon.exe" relSourcePath="Recon.exe" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="c:\Users\rtaschereau\Documents\GENISYS4_GUI 1292\dicomCT.exe" relSourcePath="dicomCT.exe" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="2" sourcePath="c:\Users\rtaschereau\Documents\GENISYS4_GUI 1292\G4_Reg_XrayCam.dll" relSourcePath="G4_Reg_XrayCam.dll" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="3" sourcePath="c:\Users\rtaschereau\Documents\GENISYS4_GUI 1292\G4_Init_XrayCam.dll" relSourcePath="G4_Init_XrayCam.dll" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="4" sourcePath="c:\Users\rtaschereau\Documents\GENISYS4_GUI 1292\FlyCapture2_C.dll" relSourcePath="FlyCapture2_C.dll" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="5" sourcePath="c:\Users\rtaschereau\Documents\GENISYS4_GUI 1292\histogram.exe" relSourcePath="histogram.exe" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="6" sourcePath="c:\Users\rtaschereau\Documents\GENISYS4_GUI 1292\PGRIdleStateFix.exe" relSourcePath="PGRIdleStateFix.exe" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="7" sourcePath="c:\Users\rtaschereau\Documents\GENISYS4_GUI 1292\dicom.exe" relSourcePath="dicom.exe" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="8" sourcePath="c:\Users\rtaschereau\Documents\GENISYS4_GUI 1292\flyiomp5md.dll" relSourcePath="flyiomp5md.dll" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="9" sourcePath="c:\Users\rtaschereau\Documents\GENISYS4_GUI 1292\conversion1276.exe" relSourcePath="conversion1276.exe" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="10" sourcePath="c:\Users\rtaschereau\Documents\GENISYS4_GUI 1292\FlyCapture2.dll" relSourcePath="FlyCapture2.dll" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="11" sourcePath="c:\Users\rtaschereau\Documents\GENISYS4_GUI 1292\X_Ray_Atlas_Registration.exe" relSourcePath="X_Ray_Atlas_Registration.exe" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="12" sourcePath="c:\Users\rtaschereau\Documents\GENISYS4_GUI 1292\cvibuild.genisys4\Release64\genisys4.exe" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="13" sourcePath="C:\Users\rtaschereau\AppData\Roaming\PETBox\NormalizationFiles\g4_calib_vial.atn" targetDir="108" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="14" sourcePath="C:\Users\rtaschereau\AppData\Roaming\PETBox\NormalizationFiles\g4_v1001_bedAndMouse.atn" targetDir="108" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="15" sourcePath="C:\Users\rtaschereau\AppData\Roaming\PETBox\NormalizationFiles\Z4_OPM1045C010_ADM1073C010.bin" targetDir="108" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput64 targetType="0" dirID="100" projectID="0">
				<fileID>12</fileID></projectOutput64>
			<projectDependencies64 dirID="100" projectID="0">
				<fileID>4</fileID></projectDependencies64></fileGroups>
		<shortcuts>
			<shortcut name="Genisys4" targetFileID="12" destDirID="101" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
		<mergemodules/>
		<products/>
		<runtimeEngine cvirte="true" instrsup="false" analysis="true" netvarsup="false" activeXsup="false" rtutilsup="false" installToAppDir="false"/>
		<advanced mediaSize="4812">
			<launchConditions>
				<condition>MINOS_WINXP</condition>
			</launchConditions>
			<includeConfigProducts>true</includeConfigProducts>
			<maxImportVisible>silent</maxImportVisible>
			<maxImportMode>merge</maxImportMode>
			<custMsgFlag>false</custMsgFlag>
			<custMsgPath>c:\program files (x86)\national instruments\cvi2010\bin\msgrte.txt</custMsgPath>
			<signExe>false</signExe>
			<certificate></certificate>
			<signTimeURL></signTimeURL>
			<signDescURL></signDescURL></advanced>
		<Projects NumProjects="1">
			<Project000 ProjectID="0" ProjectAbsolutePath="c:\Users\rtaschereau\Documents\GENISYS4_GUI 1292\genisys4.prj" ProjectRelativePath="genisys4.prj"/></Projects>
		<buildData progressBarRate="0.547845803331150">
			<progressTimes>
				<Begin>0.000000000000000</Begin>
				<ProductsAdded>0.282532732251772</ProductsAdded>
				<DPConfigured>2.301190282310463</DPConfigured>
				<DPMergeModulesAdded>162.164335175117690</DPMergeModulesAdded>
				<DPClosed>171.431464704625740</DPClosed>
				<DistributionsCopied>171.892498639470690</DistributionsCopied>
				<End>182.533113135036900</End></progressTimes></buildData>
	</msi>
</distribution>
