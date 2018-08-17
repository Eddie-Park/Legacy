<distribution version="9.0.1" name="Embedded" type="MSI">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{D59A7F2A-5392-4E71-9013-4757E86056C3}">
		<general appName="Embedded" outputLocation="x:\rtaschereau\petBoxFolder\Embedded 1102\cvidistkit.Embedded" relOutputLocation="cvidistkit.Embedded" outputLocationWithVars="x:\rtaschereau\petBoxFolder\Embedded 1102\cvidistkit.%name" relOutputLocationWithVars="cvidistkit.%name" autoIncrement="true" version="1.0.4">
			<arp company="UCLA - Sofie LLC" companyURL="" supportURL="" contact="" phone="" comments=""/>
			<summary title="" subject="" keyWords="" comments="" author=""/></general>
		<userinterface language="English" readMe="" license="">
			<dlgstrings welcomeTitle="Embedded" welcomeText=""/></userinterface>
		<dirs appDirID="101">
			<installDir name="Embedded" dirID="100" parentID="7" isMSIDir="false" visible="true"/>
			<installDir name="[Program Files]" dirID="2" parentID="-1" isMSIDir="true" visible="true"/>
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true"/>
			<installDir name="Embedded" dirID="101" parentID="2" isMSIDir="false" visible="true"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="x:\rtaschereau\petBoxFolder\Embedded 1102\embedded.exe" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="x:\rtaschereau\petBoxFolder\Embedded 1102\VHSSupport\lib\Vhsadac16_host_api.lib" relSourcePath="VHSSupport\lib\Vhsadac16_host_api.lib" relSourceBase="0" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput targetType="0" dirID="101" projectID="0">
				<fileID>0</fileID></projectOutput>
			<projectDependencies dirID="101" projectID="0"/></fileGroups>
		<registry>
			<regKey hive="MDFBDP_HKEY_LOCAL_MACHINE" path="SOFTWARE\Sofie Inc.\PETBoxServer\1.0" name="LogFileNumber" type="MDFBDP_REG_DWORD" value="0" default="false"/>
			<regKey hive="MDFBDP_HKEY_LOCAL_MACHINE" path="SOFTWARE\Sofie Inc.\PETBoxServer\1.0" name="SerialNumber" type="MDFBDP_REG_DWORD" value="1001" default="false"/>
			<regKey hive="MDFBDP_HKEY_LOCAL_MACHINE" path="SOFTWARE\Sofie Inc.\PETBoxServer\1.0" name="dicomRegID" type="MDFBDP_REG_DWORD" value="20101919" default="false"/></registry>
		<shortcuts>
			<shortcut name="Embedded" targetFileID="0" destDirID="100" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
		<mergemodules/>
		<products/>
		<runtimeEngine cvirte="true" instrsup="true" lvrt="true" analysis="true" netvarsup="true" dotnetsup="true" activeXsup="true" lowlevelsup="true" rtutilsup="true" installToAppDir="false"/>
		<advanced mediaSize="650">
			<launchConditions>
				<condition>MINOS_WIN2K_SP3</condition>
			</launchConditions>
			<includeConfigProducts>true</includeConfigProducts>
			<maxImportVisible>silent</maxImportVisible>
			<maxImportMode>merge</maxImportMode></advanced>
		<Projects NumProjects="1">
			<Project000 ProjectID="0" ProjectAbsolutePath="x:\rtaschereau\petBoxFolder\Embedded 1102\embedded.prj" ProjectRelativePath="embedded.prj"/></Projects>
		<buildData progressBarRate="0.051251958149070">
			<progressTimes>
					<Begin>0.000000000000000</Begin>
					<ProductsAdded>0.125000000000000</ProductsAdded>
					<DPConfigured>0.407000064849854</DPConfigured>
					<DPMergeModulesAdded>0.843999862670898</DPMergeModulesAdded>
					<DPClosed>2.500000000000000</DPClosed>
					<DistributionsCopied>2.546999931335449</DistributionsCopied>
					<End>19.464449701309206</End></progressTimes></buildData>
	</msi>
</distribution>
