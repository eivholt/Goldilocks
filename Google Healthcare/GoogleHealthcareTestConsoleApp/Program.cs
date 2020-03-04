using Google.Apis.Auth.OAuth2;
using Google.Apis.CloudHealthcare.v1beta1;
using Google.Apis.Services;
using Newtonsoft.Json;
using System;
using System.Threading.Tasks;
using Data = Google.Apis.CloudHealthcare.v1beta1.Data;

namespace GoogleHealthcareTestConsoleApp
{
    class Program
    {
        static void Main(string[] args)
        {
            CloudHealthcareService cloudHealthcareService = new CloudHealthcareService(new BaseClientService.Initializer
            {
                HttpClientInitializer = GetCredential(),
                ApplicationName = "Google-CloudHealthcareSample/0.1",
            });

            var projectId = "iothealthcare-269209";
            var location = "europe-west4";
            var datasetId = "IoTHealthcareDataset";
            var fhirStoreId = "LocationsDataStore";
            var resourceId = "e896887e-c3ec-4c88-aa59-f17dc942ccd0";

            // Name of the FHIR store to retrieve the capabilities for.
            //string name = $"projects/{projectId}/locations/{location}/datasets/{datasetId}/fhirStores/{fhirStoreId}";  // TODO: Update placeholder value.

            // Name of the dataset.
            string parent = $"projects/{projectId}/locations/{location}/datasets/{datasetId}";  // TODO: Update placeholder value.

            // The name of the resource to retrieve.
            string name = $"projects/{projectId}/locations/{location}/datasets/{datasetId}/fhirStores/{fhirStoreId}/fhir/Location/{resourceId}";  // TODO: Update placeholder value.

            ProjectsResource.LocationsResource.DatasetsResource.FhirStoresResource.FhirResource.ReadRequest request = cloudHealthcareService.Projects.Locations.Datasets.FhirStores.Fhir.Read(name);

            // To execute asynchronously in an async method, replace `request.Execute()` as shown:
            Data.HttpBody response = request.Execute();
            // Data.HttpBody response = await request.ExecuteAsync();


            // TODO: Change code below to process the `response` object:
            var json = JsonConvert.SerializeObject(response);
            Console.WriteLine(json);
        }

        public static GoogleCredential GetCredential()
        {
            GoogleCredential credential = Task.Run(() => GoogleCredential.GetApplicationDefaultAsync()).Result;
            if (credential.IsCreateScopedRequired)
            {
                credential = credential.CreateScoped("https://www.googleapis.com/auth/cloud-platform");
            }
            return credential;
        }
    }
}
