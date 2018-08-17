<distribution version="10.0.0" name="PET Daemon" type="MSI">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{2C762E72-C042-4559-A8E1-FD324C41B002}">
		<general appName="PET Daemon" outputLocation="c:\SHANG SHANG\PETBOX\GENISYS4 Embedded 1235\cvidistkit.PET Daemon" relOutputLocation="cvidistkit.PET Daemon" outputLocationWithVars="c:\SHANG SHANG\PETBOX\GENISYS4 Embedded 1235\cvidistkit.%name" relOutputLocationWithVars="cvidistkit.%name" upgradeBehavior="1" autoIncrement="true" version="1.0.2">
			<arp company="Sofie Biosciences" companyURL="" supportURL="" contact="" phone="" comments=""/>
			<summary title="" subject="" keyWords="" comments="" author=""/></general>
		<userinterface language="English" showPaths="true" readMe="" license="">
			<dlgstrings welcomeTitle="PET Daemon" welcomeText=""/></userinterface>
		<dirs appDirID="100">
			<installDir name="[Program Files]" dirID="2" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="PET Daemon" dirID="100" parentID="2" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="PET Daemon" dirID="101" parentID="7" isMSIDir="false" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="c:\SHANG SHANG\PETBOX\GENISYS4 Embedded 1235\cvibuild.embedded\Release\embedded.exe" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput targetType="0" dirID="100" projectID="0">
				<fileID>0</fileID></projectOutput>
			<projectDependencies dirID="100" projectID="0"/></fileGroups>
		<shortcuts>
			<shortcut name="Embedded" targetFileID="0" destDirID="101" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
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
			<Project000 ProjectID="0" ProjectAbsolutePath="c:\SHANG SHANG\PETBOX\GENISYS4 Embedded 1235\embedded.prj" ProjectRelativePath="embedded.prj"/></Projects>
		<buildData progressBarRate="0.274062370367428">
			<progressTimes>
				<Begin>0.000000000000000</Begin>
				<ProductsAdded>0.060576250000000</ProductsAdded>
				<DPConfigured>1.710512750000000</DPConfigured>
				<DPMergeModulesAdded>3.390513500000000</DPMergeModulesAdded>
				<DPClosed>9.303037750000000</DPClosed>
				<DistributionsCopied>13.015628499999996</DistributionsCopied>
				<End>364.880446249999980</End></progressTimes></buildData>
	</msi>
</distribution>
