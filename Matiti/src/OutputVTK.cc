#include <OutputVTK.h>
#include <Exception.h>
#include <NodePArray.h>
#include <Body.h>
#include <Node.h>

#include <vtkUnstructuredGrid.h>
#include <vtkHexahedron.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkXMLMultiBlockDataWriter.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkSmartPointer.h>

#include <unistd.h>
#include <fstream>
#include <regex>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

using namespace Matiti;

OutputVTK::OutputVTK()
  : Output()
{
}

OutputVTK::OutputVTK(const Uintah::ProblemSpecP& ps)
  : Output(ps)
{
}

OutputVTK::~OutputVTK()
{
}

void
OutputVTK::write(const Time& time, const Domain& domain, const BodySPArray& bodyList) 
{
  // Get the file names 
  std::ostringstream domain_of_name, node_of_name;
  getFileNames(domain_of_name, node_of_name);

  // Write files for the domain extents
  writeDomain(time, domain, domain_of_name);

  // Write files for the nodes and node quantities
  writeNodes(time, bodyList, node_of_name);

  // Increment the output file count
  incrementOutputFileCount();
}

void
OutputVTK::writeDomain(const Time& time, const Domain& domain, 
                       std::ostringstream& fileName)
{
  // Create a writer
  vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = 
     vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();

  // Get the filename
  fileName << "." << writer->GetDefaultFileExtension();
  writer->SetFileName((fileName.str()).c_str());

  // Create a pointer to a VTK Unstructured Grid data set
  vtkSmartPointer<vtkUnstructuredGrid> data_set = vtkSmartPointer<vtkUnstructuredGrid>::New();

  // Set up pointer to point data
  vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New(); 
  pts->SetNumberOfPoints(8);  

  // Add the time
  addTimeToVTKDataSet(time.currentTime(), data_set);

  // Add the domain boundary to the unstructured grid cell data
  addDomainToVTKUnstructuredGrid(domain, pts, data_set);

  // Set the points
  data_set->SetPoints(pts);

  // Remove unused memeomry
  data_set->Squeeze();

  // Write the data
  writer->SetInput(data_set);
  writer->SetDataModeToAscii();
  writer->Write();
}

void
OutputVTK::writeNodes(const Time& time, const BodySPArray& bodyList,
                      std::ostringstream& fileName) 
{
  // Create a writer
  vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = 
     vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();

  // Get the filename
  fileName << "." << writer->GetDefaultFileExtension();
  writer->SetFileName((fileName.str()).c_str());

  // Create a pointer to a VTK Unstructured Grid data set
  vtkSmartPointer<vtkUnstructuredGrid> data_set = vtkSmartPointer<vtkUnstructuredGrid>::New();

  // Set up pointer to point data
  vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New(); 
  int num_pts = 0;
  for (auto body_iter = bodyList.begin(); body_iter != bodyList.end(); ++body_iter) {
    num_pts += ((*body_iter)->nodes()).size();
  }
  pts->SetNumberOfPoints(num_pts);  

  // Add the time
  addTimeToVTKDataSet(time.currentTime(), data_set);

  // Loop through bodies
  for (auto body_iter = bodyList.begin(); body_iter != bodyList.end(); ++body_iter) {

    // Get the node list for the body
    const NodePArray& node_list = (*body_iter)->nodes();

    // Save the actual points and data
    createVTKUnstructuredGrid(node_list, pts, data_set);
  }

  // Set the points
  data_set->SetPoints(pts);

  // Remove unused memeomry
  data_set->Squeeze();

  // Write the data
  writer->SetInput(data_set);
  writer->SetDataModeToAscii();
  writer->Write();
}

void
OutputVTK::writeMB(const Time& time, const Domain& domain, const BodySPArray& bodyList) 
{
  // Create a writer
  vtkSmartPointer<vtkXMLMultiBlockDataWriter> writer = 
     vtkSmartPointer<vtkXMLMultiBlockDataWriter>::New();

  // Get the filename
  std::ostringstream of_name, of_d_name;
  getFileNames(of_d_name, of_name);
  of_name << "." << writer->GetDefaultFileExtension();
  writer->SetFileName((of_name.str()).c_str());

  // Create a pointer to a VTK MultiBlock data set
  vtkSmartPointer<vtkMultiBlockDataSet> data_set = vtkSmartPointer<vtkMultiBlockDataSet>::New();

  // Loop through bodies
  int body_count = 1;
  for (auto body_iter = bodyList.begin(); body_iter != bodyList.end(); ++body_iter) {

    // Get the node list for the body
    const NodePArray& node_list = (*body_iter)->nodes();

    // Create pointer to VTK UnstructuredGrid data set
    vtkSmartPointer<vtkUnstructuredGrid> point_data = vtkSmartPointer<vtkUnstructuredGrid>::New();

    // Add the time
    addTimeToVTKDataSet(time.currentTime(), point_data);

    // Create the unstructured data set for the body
    createVTKUnstructuredDataSet(domain, node_list, point_data);

    // Add the unstructured data to the multi block
    data_set->SetBlock(2*body_count-1, point_data);

    // Create pointer to VTK UnstructuredGrid data set
    vtkSmartPointer<vtkUnstructuredGrid> domain_data = vtkSmartPointer<vtkUnstructuredGrid>::New();

    // Add the domain boundary to the unstructured grid cell data
    addDomainToVTKDataSet(domain, domain_data);

    // Add the domain data to the multi block
    data_set->SetBlock(2*body_count, domain_data);

    // Increment body count
    ++body_count;
  }

  // Write the data
  writer->SetInput(data_set);
  writer->SetDataModeToAscii();
  writer->Write();

  // Increment the output file count
  incrementOutputFileCount();
}

void 
OutputVTK::addTimeToVTKDataSet(double time, vtkSmartPointer<vtkUnstructuredGrid>& dataSet)
{
  vtkSmartPointer<vtkDoubleArray> array = vtkSmartPointer<vtkDoubleArray>::New();
  array->SetName("TIME");
  array->SetNumberOfTuples(1);
  array->SetTuple1(0, time);
  dataSet->GetFieldData()->AddArray(array);
}

void 
OutputVTK::addDomainToVTKUnstructuredGrid(const Domain& domain, 
                                          vtkSmartPointer<vtkPoints>& pts,
                                          vtkSmartPointer<vtkUnstructuredGrid>& dataSet)
{
  Point3D lower = domain.lower();
  Point3D upper = domain.upper();
  double xmin = lower.x();
  double ymin = lower.y();
  double zmin = lower.z();
  double xmax = upper.x();
  double ymax = upper.y();
  double zmax = upper.z();

  int id = 0;
  pts->SetPoint(id, xmin, ymin, zmin); ++id;
  pts->SetPoint(id, xmax, ymin, zmin); ++id;
  pts->SetPoint(id, xmax, ymax, zmin); ++id;
  pts->SetPoint(id, xmin, ymax, zmin); ++id;
  pts->SetPoint(id, xmin, ymin, zmax); ++id;
  pts->SetPoint(id, xmax, ymin, zmax); ++id;
  pts->SetPoint(id, xmax, ymax, zmax); ++id;
  pts->SetPoint(id, xmin, ymax, zmax);

  vtkSmartPointer<vtkHexahedron> hex = vtkSmartPointer<vtkHexahedron>::New();
  for (int ii = 0; ii < 8; ++ii) {
    hex->GetPointIds()->SetId(ii, ii); 
  }
  dataSet->InsertNextCell(hex->GetCellType(), hex->GetPointIds());
}

void
OutputVTK::createVTKUnstructuredGrid(const NodePArray& nodeList, 
                                     vtkSmartPointer<vtkPoints>& pts,
                                     vtkSmartPointer<vtkUnstructuredGrid>& dataSet)
{
  // Set up pointers for material property data
  vtkSmartPointer<vtkDoubleArray> density = vtkSmartPointer<vtkDoubleArray>::New();
  density->SetNumberOfComponents(1);
  density->SetNumberOfTuples(pts->GetNumberOfPoints());
  density->SetName("Density");

  vtkSmartPointer<vtkDoubleArray> micromodulus = vtkSmartPointer<vtkDoubleArray>::New();
  micromodulus->SetNumberOfComponents(1);
  micromodulus->SetNumberOfTuples(pts->GetNumberOfPoints());
  micromodulus->SetName("Micromodulus");

  vtkSmartPointer<vtkDoubleArray> fracture_energy = vtkSmartPointer<vtkDoubleArray>::New();
  fracture_energy->SetNumberOfComponents(1);
  fracture_energy->SetNumberOfTuples(pts->GetNumberOfPoints());
  fracture_energy->SetName("FractureEnergy");

  // Set up pointer for damage data
  vtkSmartPointer<vtkDoubleArray> damage = vtkSmartPointer<vtkDoubleArray>::New();
  damage->SetNumberOfComponents(1);
  damage->SetNumberOfTuples(pts->GetNumberOfPoints());
  damage->SetName("Damage");

  // Set up pointer for displacement and velocity data
  vtkSmartPointer<vtkDoubleArray> disp = vtkSmartPointer<vtkDoubleArray>::New();
  disp->SetNumberOfComponents(3);
  disp->SetNumberOfTuples(pts->GetNumberOfPoints());
  disp->SetName("Displacement");

  vtkSmartPointer<vtkDoubleArray> vel = vtkSmartPointer<vtkDoubleArray>::New();
  vel->SetNumberOfComponents(3);
  vel->SetNumberOfTuples(pts->GetNumberOfPoints());
  vel->SetName("Velocity");

  // Loop through nodes
  double displacement[3], position[3], velocity[3];
  int id = 0;
  for (auto node_iter = nodeList.begin(); node_iter != nodeList.end(); ++node_iter) {
    NodeP cur_node = *node_iter;
    if (cur_node->omit()) continue;  // skip this node
    for (int ii = 0; ii < 3; ++ii) {
      displacement[ii] = cur_node->displacement()[ii];
      position[ii] = cur_node->position()[ii] + displacement[ii];
      velocity[ii] = cur_node->velocity()[ii];
    }
    pts->SetPoint(id, position);
    //std::cout << "Damage array = " << damage << std::endl;
    //std::cout << "size = " << nodeList.size() << "count = " << count << " id = " << id << " index = " << cur_node->damageIndex() << std::endl;
    density->InsertValue(id, cur_node->density());
    micromodulus->InsertValue(id, cur_node->material()->microModulus());
    fracture_energy->InsertValue(id, cur_node->material()->fractureEnergy());
    damage->InsertValue(id, cur_node->damageIndex());
    disp->InsertTuple(id, displacement);
    vel->InsertTuple(id, velocity);
    ++id;
  }

  // Add points to data set
  dataSet->GetPointData()->AddArray(density);
  dataSet->GetPointData()->AddArray(micromodulus);
  dataSet->GetPointData()->AddArray(fracture_energy);
  dataSet->GetPointData()->AddArray(damage);
  dataSet->GetPointData()->AddArray(disp);
  dataSet->GetPointData()->AddArray(vel);

  // Check point data
  /*
  vtkPointData *pd = dataSet->GetPointData();
  if (pd) {
    std::cout << " contains point data with " << pd->GetNumberOfArrays() << " arrays." << std::endl;
    for (int i = 0; i < pd->GetNumberOfArrays(); i++) {
      std::cout << "\tArray " << i << " is named "
                << (pd->GetArrayName(i) ? pd->GetArrayName(i) : "NULL")
                << std::endl;
    }
  }
  */
}

void
OutputVTK::createVTKUnstructuredDataSet(const Domain& domain,
                                        const NodePArray& nodeList, 
                                        vtkSmartPointer<vtkUnstructuredGrid>& dataSet)
{

  // Find number of points
  int count = 0;
  for (auto node_iter = nodeList.begin(); node_iter != nodeList.end(); ++node_iter) {
    NodeP cur_node = *node_iter;
    if (cur_node->omit()) continue;  // skip this node
    ++count;
  }

  // Set up pointer to point data
  vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New(); 
  pts->SetNumberOfPoints(count);  
  
  // Set up pointer for damage data
  vtkSmartPointer<vtkDoubleArray> damage = vtkSmartPointer<vtkDoubleArray>::New();
  damage->SetNumberOfComponents(1);
  damage->SetNumberOfTuples(count);
  damage->SetName("Damage");

  // Set up pointer for displacement and velocity data
  vtkSmartPointer<vtkDoubleArray> disp = vtkSmartPointer<vtkDoubleArray>::New();
  disp->SetNumberOfComponents(3);
  disp->SetNumberOfTuples(count);
  disp->SetName("Displacement");

  vtkSmartPointer<vtkDoubleArray> vel = vtkSmartPointer<vtkDoubleArray>::New();
  vel->SetNumberOfComponents(3);
  vel->SetNumberOfTuples(count);
  vel->SetName("Velocity");

  // Loop through nodes
  int id = 0;
  for (auto node_iter = nodeList.begin(); node_iter != nodeList.end(); ++node_iter) {
    NodeP cur_node = *node_iter;
    if (cur_node->omit()) continue;  // skip this node
    double displacement[3], position[3], velocity[3];
    for (int ii = 0; ii < 3; ++ii) {
      displacement[ii] = cur_node->displacement()[ii];
      position[ii] = cur_node->position()[ii] + displacement[ii];
      velocity[ii] = cur_node->velocity()[ii];
    }
    pts->InsertNextPoint(position);
    //std::cout << "Damage array = " << damage << std::endl;
    //std::cout << "size = " << nodeList.size() << "count = " << count << " id = " << id << " index = " << cur_node->damageIndex() << std::endl;
    damage->InsertNextValue(cur_node->damageIndex());
    disp->InsertNextTuple(displacement);
    vel->InsertNextTuple(velocity);
    ++id;
  }

  // Add points to data set
  dataSet->SetPoints(pts);
  dataSet->GetPointData()->AddArray(damage);
  dataSet->GetPointData()->AddArray(disp);
  dataSet->GetPointData()->AddArray(vel);

  // Check point data
  vtkPointData *pd = dataSet->GetPointData();
  if (pd) {
    std::cout << " contains point data with " << pd->GetNumberOfArrays() << " arrays." << std::endl;
    for (int i = 0; i < pd->GetNumberOfArrays(); i++) {
      std::cout << "\tArray " << i << " is named "
                << (pd->GetArrayName(i) ? pd->GetArrayName(i) : "NULL")
                << std::endl;
    }
  }

}

void 
OutputVTK::addDomainToVTKDataSet(const Domain& domain, 
                                 vtkSmartPointer<vtkUnstructuredGrid>& dataSet)
{
  Point3D lower = domain.lower();
  Point3D upper = domain.upper();
  double xmin = lower.x();
  double ymin = lower.y();
  double zmin = lower.z();
  double xmax = upper.x();
  double ymax = upper.y();
  double zmax = upper.z();

  // Set up pointer to point data
  vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New(); 
  pts->SetNumberOfPoints(8);  
  //int pointID = pts->GetNumberOfPoints();
  pts->InsertNextPoint(xmin, ymin, zmin);
  pts->InsertNextPoint(xmax, ymin, zmin);
  pts->InsertNextPoint(xmax, ymax, zmin);
  pts->InsertNextPoint(xmin, ymax, zmin);
  pts->InsertNextPoint(xmin, ymin, zmax);
  pts->InsertNextPoint(xmax, ymin, zmax);
  pts->InsertNextPoint(xmax, ymax, zmax);
  pts->InsertNextPoint(xmin, ymax, zmax);

  vtkSmartPointer<vtkHexahedron> hex = vtkSmartPointer<vtkHexahedron>::New();
  for (int ii = 0; ii < 8; ++ii) {
    hex->GetPointIds()->SetId(ii, ii); 
  }
  dataSet->SetPoints(pts);
  dataSet->InsertNextCell(hex->GetCellType(), hex->GetPointIds());
}

// Get individual file names
void
OutputVTK::getFileNames(std::ostringstream& domainFileName,
                        std::ostringstream& nodeFileName)
{
  std::string name_without_ext = outputFile();
  unsigned int lastIndex = name_without_ext.find_last_of(".");
  if (lastIndex != std::string::npos) {
    name_without_ext = name_without_ext.substr(0, lastIndex); 
  }

  if (outputFileCount() == 0) {
  
    d_output_dir << "./" << name_without_ext;

    #if defined(_WIN32)
      _mkdir((d_output_dir.str()).c_str());
    #else 
      int dircount = 0;
      while (opendir((d_output_dir.str()).c_str())) {
        ++dircount;
        d_output_dir.clear();
        d_output_dir.str("");
        d_output_dir << "./" << name_without_ext;
        d_output_dir << "_" << std::setfill('0') << std::setw(2) << dircount;
      }
      mkdir((d_output_dir.str()).c_str(), 0777);
    #endif

  }

  domainFileName << d_output_dir.str() << "/"
           << name_without_ext << "_d_" << std::setfill('0') << std::setw(5) << outputFileCount(); 
  nodeFileName << d_output_dir.str() << "/"
           << name_without_ext << "_" << std::setfill('0') << std::setw(5) << outputFileCount(); 
}