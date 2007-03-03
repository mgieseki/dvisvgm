
class Font
{

};


class PhysicalFont
{
   PhysicalFont (string name);
   
};


class VirtualFont : public VFActions
{
   public:
      VirtualFont (string name);
      void setChar (DVIReader &dviReader);

   private:
      map
      
};


void VirtualFont::readVF (string name) {
   ifstream ifs(name);
   if (ifs) {
      VFReader vfReader(ifs);
      vfReader.setActions(this);
      vfReader.execute();
   }
}

void VirtualFont::defineChar () {

}

void VirtualFont::defineFont () {

}


class FontManager
{
   public:
   
      void addFont (int num, string name, double dsize, double scale);
      bool isVirtual (int fontNo) const;
   private:
      vector<FontInfo> fontInfo;
      map<int,int> fontNumber; ///< DVI font number -> sequential font number       
};


void FontManager::addFont (int num, string name, double dsize, double scale) {
   
}
