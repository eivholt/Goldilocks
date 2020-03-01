using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Extensions.Http;
using Microsoft.Extensions.Logging;
using OccupancyIotaPublisherFunctionApp.Dto;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

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
            var hordeUdpMessageStreamAnalytics = HordeUdpMessageStreamAnalytics.FromJson(requestBody);

            return hordeUdpMessageStreamAnalytics != null
                ? (ActionResult)new OkObjectResult($"Hello, {hordeUdpMessageStreamAnalytics.First().DeviceId}")
                : new BadRequestObjectResult("Please pass a name on the query string or in the request body");
        }
    }
}
