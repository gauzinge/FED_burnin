/*

        FileName :                    SystemController.cc
        Content :                     Controller of the System, overall wrapper of the framework
        Programmer :                  Nicolas PIERRE
        Version :                     1.0
        Date of creation :            10/08/14
        Support :                     mail to : nicolas.pierre@cern.ch

 */

#include "SystemController.h"

using namespace Ph2_HwDescription;
using namespace Ph2_HwInterface;

namespace Ph2_System
{

	SystemController::SystemController()
	{
		fFileHandler = nullptr;
	}

	SystemController::~SystemController()
	{
		for ( auto& el : fShelveVector )
			delete el;
		fShelveVector.clear();
	}

	void SystemController::addFileHandler( std::string pFilename , char pOption )
	{

		fFileHandler = new FileHandler( pFilename, pOption );
	}

	void SystemController::readFile( std::vector<uint32_t>& pVec )
	{
		pVec = fFileHandler->readFile( );
	}

	void SystemController::InitializeHw( const std::string& pFilename, std::ostream& os )
	{
		if ( pFilename.find( ".xml" ) != std::string::npos )
			parseHWxml( pFilename, os );
		else if ( pFilename.find( ".json" ) != std::string::npos )
			parseHWjson( pFilename, os );
		else
			std::cerr << "Could not parse settings file " << pFilename << " - it is neither .xml nor .json format!" << std::endl;
		if ( fFileHandler != NULL ) std::cout << BOLDBLUE << "Saving binary raw data to: " << fFileHandler->getFilename() << RESET << std::endl;
	}

	void SystemController::InitializeSettings( const std::string& pFilename, std::ostream& os )
	{
		if ( pFilename.find( ".xml" ) != std::string::npos )
			parseSettingsxml( pFilename, os );
		else if ( pFilename.find( ".json" ) != std::string::npos )
			parseSettingsjson( pFilename, os );
		else
			std::cerr << "Could not parse settings file " << pFilename << " - it is neither .xml nor .json format!" << std::endl;
	}

	void SystemController::ConfigureHw( std::ostream& os , bool bIgnoreI2c )
	{

		bool cHoleMode, cCheck;
		if ( !fSettingsMap.empty() )
		{
			SettingsMap::iterator cSetting = fSettingsMap.find( "HoleMode" );
			if ( cSetting != fSettingsMap.end() )
			{
				cHoleMode = cSetting->second;
				os << GREEN << "Overriding GLIB register values for signal polarity with value from settings node!" << RESET << std::endl;
			}
			cCheck = true;
		}
		else cCheck = false;

		class Configurator : public HwDescriptionVisitor
		{
		  private:
			bool fHoleMode, fCheck, fIgnoreI2c;
			Ph2_HwInterface::BeBoardInterface* fBeBoardInterface;
			Ph2_HwInterface::CbcInterface* fCbcInterface;
			std::ostream& los_;
		  public:
			Configurator( Ph2_HwInterface::BeBoardInterface* pBeBoardInterface, Ph2_HwInterface::CbcInterface* pCbcInterface, bool pHoleMode, bool pCheck, bool pIgnoreI2c, std::ostream& los ): fBeBoardInterface( pBeBoardInterface ), fCbcInterface( pCbcInterface ), fHoleMode( pHoleMode ), fCheck( pCheck ), fIgnoreI2c( pIgnoreI2c ), los_( los ) {}

			void visit( BeBoard& pBoard ) {
				fBeBoardInterface->ConfigureBoard( &pBoard );

				if ( fCheck )
					fBeBoardInterface->WriteBoardReg( &pBoard, NEG_LOGIC_CBC, ( ( fHoleMode ) ? 0 : 1 ) );
				los_ << GREEN << "Successfully configured Board " << int( pBoard.getBeId() ) << RESET << std::endl;
			}

			void visit( Cbc& pCbc ) {
				if ( !fIgnoreI2c ) {
					fCbcInterface->ConfigureCbc( &pCbc );
					los_ << GREEN <<  "Successfully configured Cbc " << int( pCbc.getCbcId() ) << RESET << std::endl;
				}
			}
		};

		Configurator cConfigurator( fBeBoardInterface, fCbcInterface, cHoleMode, cCheck, bIgnoreI2c, os );
		accept( cConfigurator );
	}

	void SystemController::Run( BeBoard* pBeBoard, uint32_t pNthAcq )
	{
		fBeBoardInterface->Start( pBeBoard );
		fBeBoardInterface->ReadData( pBeBoard, pNthAcq, true );
		fBeBoardInterface->Stop( pBeBoard, pNthAcq );
	}

	void SystemController::parseHWxml( const std::string& pFilename, std::ostream& os )
	{
		pugi::xml_document doc;
		uint32_t cShelveId, cBeId, cModuleId, cCbcId;
		uint32_t cNShelve = 0;
		int i, j;

		pugi::xml_parse_result result = doc.load_file( pFilename.c_str() );

		if ( !result )
		{
			os << "ERROR :\n Unable to open the file : " << pFilename << std::endl;
			os << "Error description : " << result.description() << std::endl;
			return;
		}

		os << "\n\n\n";
		for ( i = 0; i < 80; i++ )
			os << "*";
		os << "\n";
		for ( j = 0; j < 40; j++ )
			os << " ";
		os << BOLDRED << "HW SUMMARY: " << RESET << std::endl;
		for ( i = 0; i < 80; i++ )
			os << "*";
		os << "\n";
		os << "\n";

		// Iterate the Shelve Nodes
		for ( pugi::xml_node cShelveNode = doc.child( "HwDescription" ).child( "Shelve" ); cShelveNode; cShelveNode = cShelveNode.next_sibling() )
		{
			cShelveId = cShelveNode.attribute( "Id" ).as_int();
			fShelveVector.push_back( new Shelve( cShelveId ) );

			os << BOLDCYAN << cShelveNode.name() << "  " << cShelveNode.first_attribute().name() << " :" << cShelveNode.attribute( "Id" ).value() << RESET << std:: endl;

			// Iterate the BeBoard Node
			for ( pugi::xml_node cBeBoardNode = cShelveNode.child( "BeBoard" ); cBeBoardNode; cBeBoardNode = cBeBoardNode.next_sibling() )
			{

				os << BOLDCYAN << "|" << "----" << cBeBoardNode.name() << "  " << cBeBoardNode.first_attribute().name() << " :" << cBeBoardNode.attribute( "Id" ).value() << RESET << std:: endl;

				cBeId = cBeBoardNode.attribute( "Id" ).as_int();
				BeBoard* cBeBoard = new BeBoard( cShelveId, cBeId );

				pugi::xml_node cBeBoardFWVersionNode = cBeBoardNode.child( "FW_Version" );
				uint16_t cNCbcDataSize = 0;
				cNCbcDataSize = uint16_t( cBeBoardFWVersionNode.attribute( "NCbcDataSize" ).as_int() );

				if ( cNCbcDataSize != 0 ) os << BOLDCYAN << "|" << "	" << "|" << "----" << cBeBoardFWVersionNode.name() << " NCbcDataSize: " << cNCbcDataSize  <<  RESET << std:: endl;
				cBeBoard->setNCbcDataSize( cNCbcDataSize );

				// Iterate the BeBoardRegister Nodes
				for ( pugi::xml_node cBeBoardRegNode = cBeBoardNode.child( "Register" ); cBeBoardRegNode/* != cBeBoardNode.child( "Module" )*/; cBeBoardRegNode = cBeBoardRegNode.next_sibling() )
				{
					// os << BOLDCYAN << "|" << "  " << "|" << "_____" << cBeBoardRegNode.name() << "  " << cBeBoardRegNode.first_attribute().name() << " :" << cBeBoardRegNode.attribute( "name" ).value() << RESET << std:: endl;
					cBeBoard->setReg( std::string( cBeBoardRegNode.attribute( "name" ).value() ), atoi( cBeBoardRegNode.first_child().value() ) );
				}

				fShelveVector[cNShelve]->addBoard( cBeBoard );

				if ( !std::string( cBeBoardNode.attribute( "boardType" ).value() ).compare( std::string( "GLIB" ) ) )
					fBeBoardFWMap[cBeBoard->getBeBoardIdentifier()] =  new GlibFWInterface( doc.child( "HwDescription" ).child( "Connections" ).attribute( "name" ).value(), cBeId, fFileHandler );
				else if ( !std::string( cBeBoardNode.attribute( "boardType" ).value() ).compare( std::string( "CTA" ) ) )
					fBeBoardFWMap[cBeBoard->getBeBoardIdentifier()] =  new CtaFWInterface( doc.child( "HwDescription" ).child( "Connections" ).attribute( "name" ).value(), cBeId, fFileHandler );
				/*else
				        cBeBoardFWInterface = new OtherFWInterface();*/

				// Iterate the module node
				for ( pugi::xml_node cModuleNode = cBeBoardNode.child( "Module" ); cModuleNode; cModuleNode = cModuleNode.next_sibling() )
				{
					if ( static_cast<std::string>( cModuleNode.name() ) == "Module" )
					{
						bool cStatus = cModuleNode.attribute( "Status" ).as_bool();
						//std::cout << cStatus << std::endl;
						if ( cStatus )
						{
							os << BOLDCYAN << "|" << "	" << "|" << "----" << cModuleNode.name() << "  " << cModuleNode.first_attribute().name() << " :" << cModuleNode.attribute( "ModuleId" ).value() << RESET << std:: endl;

							cModuleId = cModuleNode.attribute( "ModuleId" ).as_int();

							Module* cModule = new Module( cShelveId, cBeId, cModuleNode.attribute( "FMCId" ).as_int(), cModuleNode.attribute( "FeId" ).as_int(), cModuleId );
							fShelveVector[cNShelve]->getBoard( cBeId )->addModule( cModule );

							pugi::xml_node cCbcPathPrefixNode = cModuleNode.child( "CBC_Files" );
							std::string cFilePrefix = std::string( cCbcPathPrefixNode.attribute( "path" ).value() );
							if ( !cFilePrefix.empty() ) os << GREEN << "|" << "	" << "|" << "	" << "|" << "----" << "CBC Files Path : " << cFilePrefix << RESET << std::endl;

							// Iterate the CBC node
							for ( pugi::xml_node cCbcNode = cModuleNode.child( "CBC" ); cCbcNode; cCbcNode = cCbcNode.next_sibling() )
							{
								os << BOLDCYAN << "|" << "	" << "|" << "	" << "|" << "----" << cCbcNode.name() << "  " << cCbcNode.first_attribute().name() << " :" << cCbcNode.attribute( "Id" ).value() << ", File: " << cCbcNode.attribute( "configfile" ).value() << RESET << std:: endl;


								std::string cFileName;
								if ( !cFilePrefix.empty() )
									cFileName = cFilePrefix + cCbcNode.attribute( "configfile" ).value();
								else cFileName = cCbcNode.attribute( "configfile" ).value();

								Cbc* cCbc = new Cbc( cShelveId, cBeId, cModuleNode.attribute( "FMCId" ).as_int(), cModuleNode.attribute( "FeId" ).as_int(), cCbcNode.attribute( "Id" ).as_int(), cFileName );

								for ( pugi::xml_node cCbcRegisterNode = cCbcNode.child( "Register" ); cCbcRegisterNode; cCbcRegisterNode = cCbcRegisterNode.next_sibling() )
									cCbc->setReg( std::string( cCbcRegisterNode.attribute( "name" ).value() ), atoi( cCbcRegisterNode.first_child().value() ) );

								for ( pugi::xml_node cCbcGlobalNode = cModuleNode.child( "Global_CBC_Register" ); cCbcGlobalNode != cModuleNode.child( "CBC" ) && cCbcGlobalNode != cModuleNode.child( "CBC_Files" ) && cCbcGlobalNode != nullptr; cCbcGlobalNode = cCbcGlobalNode.next_sibling() )
								{

									if ( cCbcGlobalNode != nullptr )
									{
										std::string regname = std::string( cCbcGlobalNode.attribute( "name" ).value() );
										uint32_t regvalue = convertAnyInt( cCbcGlobalNode.first_child().value() ) ;
										cCbc->setReg( regname, uint8_t( regvalue ) ) ;

										os << GREEN << "|" << "	" << "|" << "	" << "|" << "----" << cCbcGlobalNode.name() << "  " << cCbcGlobalNode.first_attribute().name() << " :" << regname << " =  0x" << std::hex << std::setw( 2 ) << std::setfill( '0' ) << regvalue << std::dec << RESET << std:: endl;
									}
								}
								fShelveVector[cNShelve]->getBoard( cBeId )->getModule( cModuleId )->addCbc( cCbc );
							}
						}
					}
				}

			}

			cNShelve++;
		}

		fBeBoardInterface = new BeBoardInterface( fBeBoardFWMap );
		fCbcInterface = new CbcInterface( fBeBoardFWMap );
		os << "\n";
		os << "\n";
		for ( i = 0; i < 80; i++ )
			os << "*";
		os << "\n";
		for ( j = 0; j < 40; j++ )
			os << " ";
		os << BOLDRED << "END OF HW SUMMARY: " << RESET << std::endl;
		for ( i = 0; i < 80; i++ )
			os << "*";
		os << "\n";
		os << "\n";
	}

	void SystemController::parseHWjson( const std::string& pFilename, std::ostream& os )
	{
		uint32_t cShelveId, cBeId, cModuleId, cCbcId;
		uint32_t cNShelve = 0;
		int i, j;

		// serialize  file into char array
		std::ifstream cIn( pFilename );
		std::string cContents( ( std::istreambuf_iterator<char>( cIn ) ), std::istreambuf_iterator<char>() );
		const char* cJsonSource = cContents.c_str() ;

		picojson::value  cJsonValue;

		std::string cErr = picojson::parse( cJsonValue, cJsonSource, cJsonSource + strlen( cJsonSource ) );
		if ( !cErr.empty() ) os << "JSON parsing errors: " << cErr << std::endl;

		os << "\n\n\n";
		for ( i = 0; i < 80; i++ )
			os << "*";
		os << "\n";
		for ( j = 0; j < 40; j++ )
			os << " ";
		os << BOLDRED << "HW SUMMARY: " << RESET << std::endl;
		for ( i = 0; i < 80; i++ )
			os << "*";
		os << "\n";
		os << "\n";

		// get the array of shelves
		picojson::array cShelves = cJsonValue.get( "HwDescription" ).get( "Shelves" ).get<picojson::array>();
		// iterate shelves
		for ( const auto& cShelve : cShelves )
		{
			cShelveId = static_cast<uint32_t>( cShelve.get( "Id" ).get<double>() );
			fShelveVector.push_back( new Shelve( cShelveId ) );
			os << BOLDCYAN << "Shelve" << "  " << "Id" << " :" << cShelveId << RESET << std:: endl;

			// get the BeBoardArray
			picojson::array cBeBoards = cShelve.get( "BeBoards" ).get<picojson::array>();
			// iterate the BeBoards
			for ( const auto& cBoard : cBeBoards )
			{
				cBeId = static_cast<uint32_t>( cBoard.get( "Id" ).get<double>() );
				BeBoard* cBeBoard = new BeBoard( cShelveId, cBeId );
				os << BOLDCYAN << "|" << "----" << "BeBoard" << "  " << "Id" << " :" << cBeId << RESET << std:: endl;

				// Iterate the BeBoardRegister Nodes
				for ( const auto& cRegister :  cBoard.get( "RegisterName" ).get<picojson::object>( ) )
				{
					// os << BOLDCYAN << "|" << "  " << "|" << "_____" << cRegister.first  << " :" << static_cast<uint32_t>( cRegister.second.get<double>() ) << RESET << std:: endl;
					cBeBoard->setReg( cRegister.first , static_cast<uint32_t>( cRegister.second.get<double>() ) );
				}

				fShelveVector[cNShelve]->addBoard( cBeBoard );

				// now need to find out what the boardType is to load the apropriate FWInterface
				BeBoardFWInterface* cBeBoardFWInterface;

				if ( cBoard.get( "boardType" ).get<std::string>() == "Glib" )
				{
					cBeBoardFWInterface = new GlibFWInterface( cJsonValue.get( "HwDescription" ).get( "Connections" ).get<std::string>().c_str(), cBeId, fFileHandler );
					fBeBoardFWMap[cBeBoard->getBeBoardIdentifier()] = cBeBoardFWInterface;
				}
				else if ( cBoard.get( "boardType" ).get<std::string>() == "Cta" )
					fBeBoardFWMap[cBeBoard->getBeBoardIdentifier()] =  new CtaFWInterface( cJsonValue.get( "HwDescription" ).get( "Connections" ).get<std::string>().c_str(), cBeId, fFileHandler );

				// now grab the modules
				picojson::array cModules = cBoard.get( "Modules" ).get<picojson::array>();
				for ( const auto& cModuleNode : cModules )
				{
					cModuleId = static_cast<uint32_t>( cModuleNode.get( "ModuleId" ).get<double>() );
					uint32_t cFMCId = static_cast<uint32_t>( cModuleNode.get( "FMCId" ).get<double>() );
					uint32_t cFeId = static_cast<uint32_t>( cModuleNode.get( "FeId" ).get<double>() );
					bool cStatus = cModuleNode.get( "Status" ).evaluate_as_boolean();

					if ( cStatus )
					{
						Module* cModule = new Module( cShelveId, cBeId, cFMCId, cFeId, cModuleId );
						fShelveVector[cNShelve]->getBoard( cBeId )->addModule( cModule );
						os << BOLDCYAN << "|" << "	" << "|" << "----" << "Module" << "  " << "FeId" << " :" << cFeId <<  RESET << std:: endl;

						// get the CBC file path prefix only if it exists!

						std::string cFilePrefix;
						// conditional parsing
						if ( !cModuleNode.get( "CbcFilePath" ).is<picojson::null>() ) cFilePrefix = cModuleNode.get( "CbcFilePath" ).get<std::string>();
						if ( !cFilePrefix.empty() ) os << GREEN << "|" << "	" << "|" << "	" << "|" << "----" << "CBC Files Path : " << cFilePrefix << RESET << std::endl;

						// get the CBC array
						picojson::array cCbcs = cModuleNode.get( "CBCs" ).get<picojson::array>();

						// Iterate the CBC node
						for ( const auto& cCbcNode : cCbcs )
						{
							cCbcId = static_cast<uint32_t>( cCbcNode.get( "Id" ).get<double>() );
							std::string cFileName;
							if ( !cFilePrefix.empty() )
								cFileName = cFilePrefix + cCbcNode.get( "configfile" ).get<std::string>();
							else cFileName = cCbcNode.get( "configfile" ).get<std::string>();
							Cbc* cCbc = new Cbc( cShelveId, cBeId, cFMCId , cFeId, cCbcId, cFileName );
							os << BOLDCYAN << "|" << "	" << "|" << "	" << "|" << "----" << "CBC" << "  " << "Id" << " :" << cCbcId << ", File: " << cFileName << RESET << std:: endl;

							// check if there is a CBC Register node
							if ( !cCbcNode.get( "Register" ).is<picojson::null>() )
							{
								// iterate the registers
								for ( const auto& cRegister :  cCbcNode.get( "Register" ).get<picojson::object>( ) )
									cCbc->setReg( cRegister.first , convertAnyInt( cRegister.second.get<std::string>().c_str() ) );
							}

							// now do the same with global registers
							if ( !cModuleNode.get( "Global_CBC_Registers" ).is<picojson::null>() )
							{
								// iterate the registers
								for ( const auto& cRegister :  cModuleNode.get( "Global_CBC_Registers" ).get<picojson::object>( ) )
								{
									std::string regname = cRegister.first;
									uint32_t regvalue = convertAnyInt( cRegister.second.get<std::string>().c_str() );
									cCbc->setReg( regname , uint8_t( regvalue ) );
									os << GREEN << "|" << "	" << "|" << "	" << "|" << "----" << "Global_CBC_Register"  << " : " << regname << " =  0x" << std::hex << std::setw( 2 ) << std::setfill( '0' ) << regvalue << std::dec << RESET << std:: endl;
								}
							}

							fShelveVector[cNShelve]->getBoard( cBeId )->getModule( cModuleId )->addCbc( cCbc );
						}
					}
				}
			}
			cNShelve++;
		}


		fBeBoardInterface = new BeBoardInterface( fBeBoardFWMap );
		fCbcInterface = new CbcInterface( fBeBoardFWMap );

		os << "\n";
		os << "\n";
		for ( i = 0; i < 80; i++ )
			os << "*";
		os << "\n";
		for ( j = 0; j < 40; j++ )
			os << " ";
		os << BOLDRED << "END OF HW SUMMARY: " << RESET << std::endl;
		for ( i = 0; i < 80; i++ )
			os << "*";
		os << "\n";
		os << "\n";
	}

	void SystemController::parseSettingsxml( const std::string& pFilename, std::ostream& os )
	{
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file( pFilename.c_str() );

		if ( !result )
		{
			os << "ERROR :\n Unable to open the file : " << pFilename << std::endl;
			os << "Error description : " << result.description() << std::endl;
			return;
		}
		for ( pugi::xml_node nSettings = doc.child( "Settings" ); nSettings; nSettings = nSettings.next_sibling() )
		{
			for ( pugi::xml_node nSetting = nSettings.child( "Setting" ); nSetting; nSetting = nSetting.next_sibling() )
			{
				fSettingsMap[nSetting.attribute( "name" ).value()] = convertAnyInt( nSetting.first_child().value() );
				os << RED << "Setting" << RESET << " --" << BOLDCYAN << nSetting.attribute( "name" ).value() << RESET << ":" << BOLDYELLOW << convertAnyInt( nSetting.first_child().value() ) << RESET << std:: endl;
			}
		}
	}

	void SystemController::parseSettingsjson( const std::string& pFilename, std::ostream& os )
	{
		// serialize  file into char array
		std::ifstream cIn( pFilename );
		std::string cContents( ( std::istreambuf_iterator<char>( cIn ) ), std::istreambuf_iterator<char>() );
		const char* cJsonSource = cContents.c_str() ;

		picojson::value  cJsonValue;

		std::string cErr = picojson::parse( cJsonValue, cJsonSource, cJsonSource + strlen( cJsonSource ) );
		if ( !cErr.empty() ) os << "JSON parsing errors: " << cErr << std::endl;

		for ( const auto& cSetting :  cJsonValue.get( "Settings" ).get<picojson::object>( ) )
		{
			fSettingsMap[cSetting.first] = static_cast<int>( cSetting.second.get<double>() );
			os << RED << "Setting" << RESET << " --" << BOLDCYAN << cSetting.first << RESET << ":" << BOLDYELLOW << static_cast<int>( cSetting.second.get<double>() ) << RESET << std:: endl;
		}
	}
}
