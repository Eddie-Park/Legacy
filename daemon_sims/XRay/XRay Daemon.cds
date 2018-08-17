<distribution version="10.0.0" name="XRay Daemon" type="MSI">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{A302F641-59DC-425E-8979-7C4071538C18}">
		<general appName="XRay Daemon" outputLocation="z:\Transfer Folder\for Emb PC_Install\Xray_Daemon\X-ray Daemon 0927\Code\cvidistkit.XRay Daemon" relOutputLocation="cvidistkit.XRay Daemon" outputLocationWithVars="z:\Transfer Folder\for Emb PC_Install\Xray_Daemon\X-ray Daemon 0927\Code\cvidistkit.%name" relOutputLocationWithVars="cvidistkit.%name" upgradeBehavior="1" autoIncrement="true" version="1.0.3">
			<arp company="Sofie Biosciences" companyURL="" supportURL="" contact="" phone="" comments=""/>
			<summary title="" subject="" keyWords="" comments="" author=""/></general>
		<userinterface language="English" showPaths="false" readMe="" license="">
			<dlgstrings welcomeTitle="XRay Daemon" welcomeText=""/></userinterface>
		<dirs appDirID="100">
			<installDir name="[Program Files]" dirID="2" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="XRay Daemon" dirID="100" parentID="2" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="XRay Daemon" dirID="101" parentID="7" isMSIDir="false" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="c:\Users\Nam\Dropbox\Genisys 4\Software\G4 Acquisition\X-ray Daemon 0927\X-ray Daemon 0927\Code\PGRFlyCapture.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="c:\Users\Nam\Dropbox\Genisys 4\Software\G4 Acquisition\X-ray Daemon 0927\X-ray Daemon 0927\Code\Scilib21.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="2" sourcePath="c:\Users\Nam\Dropbox\Genisys 4\Software\G4 Acquisition\X-ray Daemon 0927\X-ray Daemon 0927\Code\SnapSDK.dll" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="3" sourcePath="z:\Transfer Folder\for Emb PC_Install\Xray_Daemon\X-ray Daemon 0927\Code\cvibuild.XRay Daemon\Release\XRay Daemon.exe" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput targetType="0" dirID="100" projectID="0">
				<fileID>3</fileID></projectOutput>
			<projectDependencies dirID="100" projectID="0">
				<fileID>0</fileID>
				<fileID>1</fileID>
				<fileID>2</fileID></projectDependencies></fileGroups>
		<shortcuts>
			<shortcut name="XRay Daemon" targetFileID="3" destDirID="101" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
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
			<custMsgPath>c:\program files (x86)\national instruments\cvi2010\bin\msgrte.txt</custMsgPath>
			<signExe>false</signExe>
			<certificate></certificate>
			<signTimeURL></signTimeURL>
			<signDescURL></signDescURL></advanced>
		<Projects NumProjects="1">
			<Project000 ProjectID="0" ProjectAbsolutePath="z:\Transfer Folder\for Emb PC_Install\Xray_Daemon\X-ray Daemon 0927\Code\XRay Daemon.prj" ProjectRelativePath="XRay Daemon.prj"/></Projects>
		<buildData progressBarRate="0.309171181305670">
			<progressTimes>
				<Begin>0.000000000000000</Begin>
				<ProductsAdded>0.120777750000005</ProductsAdded>
				<DPConfigured>1.914549625000001</DPConfigured>
				<DPMergeModulesAdded>3.185776250000004</DPMergeModulesAdded>
				<DPClosed>7.067102500000003</DPClosed>
				<DistributionsCopied>16.679219874999998</DistributionsCopied>
				<End>323.445411625000020</End></progressTimes></buildData>
	</msi>
</distribution>
