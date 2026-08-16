#ifndef CUSTOM_DETECTOR_HH
#define CUSTOM_DETECTOR_HH

#include <string>
#include <vector>

class G4Material;
class G4VSolid;
class G4LogicalVolume;
class G4VPhysicalVolume;

class CustomDetector {
    public:
        void Construct(
            G4LogicalVolume* mother,
            std::string name
        );
    
        virtual ~CustomDetector() = default;

        G4Material* GetMaterial() const {
            return fMaterial;
        }
        G4VSolid* GetSolid() const {
            return fSolid;
        }
        G4LogicalVolume* GetLogical() const {
            return fLogical;
        }
        std::vector<G4VPhysicalVolume*> GetPhysical() const {
            return fPhysical;
        }

    protected:
        virtual G4Material* CreateMaterial() = 0;
        virtual G4VSolid* CreateSolid(std::string name) = 0;

        virtual std::vector<G4VPhysicalVolume*> Place(
            G4LogicalVolume* mother,
            G4LogicalVolume* logical,
            std::string name
        ) = 0;

    private:
        std::string fName;
        G4Material* fMaterial = nullptr;
        G4VSolid* fSolid = nullptr;
        G4LogicalVolume* fLogical = nullptr;
        std::vector<G4VPhysicalVolume*> fPhysical;
};

#endif