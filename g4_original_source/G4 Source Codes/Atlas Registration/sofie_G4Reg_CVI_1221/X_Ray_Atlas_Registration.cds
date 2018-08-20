<distribution version="10.0.0" name="X_Ray_Atlas_Registration" type="MSI">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{DFEC8993-9162-4281-925B-23E4EA41DFC5}">
		<general appName="X_Ray_Atlas_Registration" outputLocation="c:\SHANG SHANG\Atlas Registration\Code\sofie_G4Reg_CVI_1221\cvidistkit.X_Ray_Atlas_Registration" relOutputLocation="cvidistkit.X_Ray_Atlas_Registration" outputLocationWithVars="c:\SHANG SHANG\Atlas Registration\Code\sofie_G4Reg_CVI_1221\cvidistkit.%name" relOutputLocationWithVars="cvidistkit.%name" upgradeBehavior="1" autoIncrement="true" version="1.0.1">
			<arp company="Sofie" companyURL="" supportURL="" contact="" phone="" comments=""/>
			<summary title="" subject="" keyWords="" comments="" author=""/></general>
		<userinterface language="English" showPaths="true" readMe="" license="">
			<dlgstrings welcomeTitle="X_Ray_Atlas_Registration" welcomeText=""/></userinterface>
		<dirs appDirID="100">
			<installDir name="[Program Files]" dirID="2" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="X_Ray_Atlas_Registration" dirID="100" parentID="2" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="X_Ray_Atlas_Registration" dirID="101" parentID="7" isMSIDir="false" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="c:\SHANG SHANG\Atlas Registration\Code\sofie_G4Reg_CVI_1221\cvibuild.X_Ray_Atlas_Registration\Release\X_Ray_Atlas_Registration.exe" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="C:\SHANG SHANG\Atlas Registration\Code\sofie_G4Reg_CVI_1221\G4_Reg_XrayCam.dll" relSourcePath="G4_Reg_XrayCam.dll" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput targetType="0" dirID="100" projectID="0">
				<fileID>0</fileID></projectOutput>
			<projectDependencies dirID="100" projectID="0">
				<fileID>1</fileID></projectDependencies></fileGroups>
		<shortcuts>
			<shortcut name="X_Ray_Atlas_Registration" targetFileID="0" destDirID="101" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
		<mergemodules/>
		<products/>
		<runtimeEngine cvirte="true" instrsup="true" lvrt="true" analysis="true" netvarsup="true" dotnetsup="true" activeXsup="true" lowlevelsup="true" rtutilsup="true" installToAppDir="false"/>
		<advanced mediaSize="650">
			<launchConditions>
				<condition>MINOS_WINXP_SP0</condition>
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
			<Project000 ProjectID="0" ProjectAbsolutePath="c:\SHANG SHANG\Atlas Registration\Code\sofie_G4Reg_CVI_1221\X_Ray_Atlas_Registration.prj" ProjectRelativePath="X_Ray_Atlas_Registration.prj"/></Projects>
		<buildData progressBarRate="0.665811117136110">
			<progressTimes>
				<Begin>0.000000000000000</Begin>
				<ProductsAdded>0.060255000000000</ProductsAdded>
				<DPConfigured>1.335255000000000</DPConfigured>
				<DPMergeModulesAdded>3.745255499999999</DPMergeModulesAdded>
				<DPClosed>12.745597499999999</DPClosed>
				<DistributionsCopied>16.420258999999998</DistributionsCopied>
				<End>150.192746000000000</End></progressTimes></buildData>
	</msi>
</distribution>
