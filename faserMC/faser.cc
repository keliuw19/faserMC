// adapted from Geant4 example

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "FTFP_BERT.hh"
#include "FaserVisAction.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "Randomize.hh"

#include "FaserDetectorConstruction.hh"
#include "FaserActionInitialization.hh"
#include "FaserPhysicsList.hh"
#include "RootEventIO.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  // Detect interactive mode (if no arguments) and define UI session
  //
  G4UIExecutive* ui = 0;
  if ( argc == 1 ) {
    ui = new G4UIExecutive(argc, argv);
  }

  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the default run manager
  //
#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
#else
  G4RunManager* runManager = new G4RunManager;
#endif

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Set mandatory initialization classes
  //
  // Detector construction
  auto * detectorConstruction = new FaserDetectorConstruction;
  runManager->SetUserInitialization(detectorConstruction);

  // Physics list
  G4VModularPhysicsList* physicsList = new TFTFP_BERT<FaserPhysicsList>();
  physicsList->SetVerboseLevel(1);
  UImanager->ApplyCommand("/control/execute faserPhysicsList.mac");  
  runManager->SetUserInitialization(physicsList);

  // User action initialization
  runManager->SetUserInitialization(new FaserActionInitialization());

  // Initialize visualization
  //
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();
  if (ui)
  {
     visManager->RegisterEndOfEventUserVisAction("FaserSpacePoints", new FaserVisAction(), G4VisExtent());
  }

  // Process macro or start UI session
  //
  if ( ! ui ) { 
    // batch mode
    if (argc > 2) RootEventIO::SetFileName(argv[2]);
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else { 
    // interactive mode
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted 
  // in the main() program !
  delete visManager;
  delete runManager;

  G4cout << "faserMC exiting" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
