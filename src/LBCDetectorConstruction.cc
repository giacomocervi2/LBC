#include "LBCDetectorConstruction.hh"

LBCDetectorConstruction::LBCDetectorConstruction()
{
}

LBCDetectorConstruction::~LBCDetectorConstruction()
{
}

G4VPhysicalVolume *LBCDetectorConstruction::Construct()
{
	G4bool checkOverlaps = true;

	//materials definition
	G4NistManager *nist = G4NistManager::Instance();
	G4Material *worldMat = nist->FindOrBuildMaterial("G4_AIR");
	G4Material *AlMat = nist->FindOrBuildMaterial("G4_Al");
	G4Material *glassMat = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
	G4Material * caseMat = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
	G4Material *plasticMat = nist->FindOrBuildMaterial("G4_PLEXIGLASS");
	
	//LBC material definition
	G4Element *elLa = nist->FindOrBuildElement("La");
	G4Element *elBr = nist->FindOrBuildElement("Br");
	G4Element *elCe = nist->FindOrBuildElement("Ce");
	G4Element *elCl = nist->FindOrBuildElement("Cl");
	G4double densityLBC = 4.90 * g / cm3; //from SCIONIX website
	G4int nComponents = 4; 
	G4Material *detMat = new G4Material("LBC", densityLBC, nComponents); 
	detMat->AddElement(elLa, 0.385085); 
	detMat->AddElement(elBr, 0.631321);
	detMat->AddElement(elCl,0.014729);
	detMat->AddElement(elCe, 0.000389);

	
	//World
	G4double xWorld = 1. * m;
	G4double yWorld = 1. * m;
	G4double zWorld = 1. * m;

	G4Box *solidWorld = new G4Box("solidworld",0.5 * xWorld, 0.5 * yWorld, 0.5 * zWorld);
	G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicalVolume");
	G4VPhysicalVolume *physWorld = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), logicWorld, "physWorld", 0, false, 0, checkOverlaps);



	//Aluminum placement
	G4double Al_z = 21.5 * mm;
	G4double Al_rMin = 0. * mm;
	G4double Al_rMax = 23 * mm;
	G4double Al_SPhi = 0. * deg;
	G4double Al_DPhi = 360. * deg;
	
	G4Tubs *solidAl = new G4Tubs("solidAl", Al_rMin, Al_rMax, Al_z, Al_SPhi, Al_DPhi);
	G4LogicalVolume *logicAl = new G4LogicalVolume(solidAl, AlMat, "logicAl");
	G4VPhysicalVolume *physAl = new G4PVPlacement(0, G4ThreeVector(0., 0., -Al_z - 2. * mm), logicAl, "physAl", logicWorld, false, 0, checkOverlaps); //0.2 è lo spessore del case

	G4VisAttributes *AlVisAtt = new G4VisAttributes(G4Color(1.0, 0.0, 0.0, 0.5));
	AlVisAtt->SetForceSolid(true);
	logicAl->SetVisAttributes(AlVisAtt);

	
	
	
	//LBC crystal placement
	G4double LBC_z = 19 * mm;
	G4double LBC_rMin = 0. * mm;
	G4double LBC_rMax = 19 * mm;
	G4double LBC_SPhi = 0. * deg;
	G4double LBC_DPhi = 360. * deg;
	
	G4Tubs *solidDetector = new G4Tubs("solidDetector", LBC_rMin, LBC_rMax, LBC_z, LBC_SPhi, LBC_DPhi);
	logicDetector = new G4LogicalVolume(solidDetector, detMat, "logicDetector");
	G4VPhysicalVolume *physDetector = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicDetector, "physDetector", logicAl, false, 0, checkOverlaps);

	G4VisAttributes *detVisAtt = new G4VisAttributes(G4Color(1.0, 1., 0.0, 1));
	detVisAtt->SetForceSolid(true);
	logicDetector->SetVisAttributes(detVisAtt);



	//source placement
	G4double thickness = 1 * mm; 
	G4double side1 = 0.5 * cm;
	G4double side2 = 1 * cm;
	G4Box *solidPlastic = new G4Box("solidPlastic", thickness, side1, side2);
	G4LogicalVolume *logicPlastic = new G4LogicalVolume(solidPlastic, plasticMat, "logicPlastic");
	G4VPhysicalVolume *physPlastic = new G4PVPlacement(0, G4ThreeVector(-1.3 * cm, 0., 8. * cm), logicPlastic,"physPlastic", logicWorld, false, 0, checkOverlaps);

	G4VisAttributes *plasticVisAtt = new G4VisAttributes(G4Color());
	plasticVisAtt->SetForceSolid(true);
	logicPlastic->SetVisAttributes(plasticVisAtt);

	//plastic case placement
	G4RotationMatrix *rotation = new G4RotationMatrix();
	rotation->rotateX(-90. *deg);
	auto mesh = CADMesh::TessellatedMesh::FromSTL("/home/cervig/geant4_LBC/HOLDER_FRONT_CAP.stl");
	auto solidCase = mesh->GetSolid();
	G4LogicalVolume *logicCase = new G4LogicalVolume(solidCase, caseMat, "logicCase");
	G4VPhysicalVolume *physCase = new G4PVPlacement(rotation, G4ThreeVector(0.,0.,-10 * cm), logicCase, "physCase", logicWorld, false, 0, checkOverlaps);

	G4VisAttributes *caseVisAtt = new G4VisAttributes(G4Color(0.5,0.5,0.5));
	caseVisAtt->SetForceWireframe(true);
	logicCase->SetVisAttributes(caseVisAtt);
	
	return physWorld;

}

void LBCDetectorConstruction::ConstructSDandField()
{
	LBCSensitiveDetector *sensDet = new LBCSensitiveDetector("SensitiveDetector");
	logicDetector->SetSensitiveDetector(sensDet);
	G4SDManager::GetSDMpointer()->AddNewDetector(sensDet);
}
