using Google.Apis.Auth.OAuth2;
using Google.Apis.CloudHealthcare.v1beta1;
using Google.Apis.Services;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Azure.KeyVault;
using Microsoft.Azure.Services.AppAuthentication;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Extensions.Http;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json;
using OccupancyIotaPublisherFunctionApp.Dto;
using System;
using System.IO;
using System.Linq;
using System.Net.Http.Headers;
using System.Threading.Tasks;
using Data = Google.Apis.CloudHealthcare.v1beta1.Data;

namespace OccupancyIotaPublisherFunctionApp
{
    public static class OccupancyPublishToIota
    {
        [FunctionName("OccupancyPublishToIotaFunction")]
        public static async Task<IActionResult> Run(
            [HttpTrigger(AuthorizationLevel.Function, "get", "post", Route = null)] HttpRequest req,
            ILogger log)
        {
            log.LogInformation("OccupancyPublishToIotaFunction HTTP trigger function processed a request.");

            string requestBody = await new StreamReader(req.Body).ReadToEndAsync();
            // Message from Stream Analytics may contain several device messages, only the first is handled in this PoC.
            var hordeUdpMessageStreamAnalytics = HordeUdpMessageStreamAnalytics.FromJson(requestBody);

            try
            {
                using (var cloudHealthcareService = new CloudHealthcareService(new BaseClientService.Initializer
                {
                    HttpClientInitializer = GetCredential(),
                    ApplicationName = "Google-CloudHealthcareSample/0.1" }))
                {

                    cloudHealthcareService.HttpClient.DefaultRequestHeaders
                      .Accept
                      .Add(new MediaTypeWithQualityHeaderValue("application/json-patch+json"));

                    var projectId = "iothealthcare-269209";
                    var location = "europe-west4";
                    var datasetId = "IoTHealthcareDataset";
                    var fhirStoreId = "LocationsDataStore";
                    var resourceId = "dfdbe4f6-a94c-420e-9e42-da5f260a31b7";

                    // The name of the resource to retrieve.
                    string name = $"projects/{projectId}/locations/{location}/datasets/{datasetId}/fhirStores/{fhirStoreId}/fhir/Location/{resourceId}";

                    Data.HttpBody patchRequestBody = new Data.HttpBody();
                    patchRequestBody.ContentType = "application/json-patch+json";
                    patchRequestBody.Data = GetPatchForBedStatus(hordeUdpMessageStreamAnalytics.First());

                    ProjectsResource.LocationsResource.DatasetsResource.FhirStoresResource.FhirResource.PatchRequest request = cloudHealthcareService.Projects.Locations.Datasets.FhirStores.Fhir.Patch(patchRequestBody, name);

                    // To execute asynchronously in an async method, replace `request.Execute()` as shown:
                    Data.HttpBody response = request.Execute();
                    var json = JsonConvert.SerializeObject(response);
                    log.LogInformation("OccupancyPublishToIotaFunction HTTP trigger function processed a request.");

                    return new ObjectResult(response);
                }
            }
            catch (Exception e)
            {
                log.LogError("e.Message");
                new BadRequestObjectResult(e.Message);
            }

            return (ActionResult)new OkObjectResult($"Posted telemetry to Google Healthcare.");
        }

        private static string GetPatchForBedStatus(HordeUdpMessageStreamAnalytics hordeUdpMessageStreamAnalytics)
        {
            return hordeUdpMessageStreamAnalytics.PayloadInt == 1 ? UnoccupiedBedPatch : OccupiedBedPatch;
        }

        public static GoogleCredential GetCredential()
        {
            var azureServiceTokenProvider = new AzureServiceTokenProvider();
            var keyVaultClient = new KeyVaultClient(
                new KeyVaultClient.AuthenticationCallback(azureServiceTokenProvider.KeyVaultTokenCallback));
            var googleProfile = keyVaultClient.GetSecretAsync(Environment.GetEnvironmentVariable("SecretIdGoogleProfile")).Result.Value;
            var credential = GoogleCredential.FromJson(googleProfile);
            if (credential.IsCreateScopedRequired)
            {
                credential = credential.CreateScoped("https://www.googleapis.com/auth/cloud-platform");
            }
            return credential;
        }

        private static string OccupiedBedPatch = @"[
                  {
                    'op': 'replace',
                    'path': '/operationalStatus',
                    'value': 
                    {
                                'code':  'O',
                        'display':  'Occupied',
                        'system':  'http://hl7.org/fhir/v2/0116',
                        'version':  '2.8.2'
                    }
                  }
                ]";

        private static string UnoccupiedBedPatch = @"[
                  {
                    'op': 'replace',
                    'path': '/operationalStatus',
                    'value': 
                    {
                                'code':  'U',
                        'display':  'Unoccupied',
                        'system':  'http://hl7.org/fhir/v2/0116',
                        'version':  '2.8.2'
                    }
                  }
                ]";
    }
}
