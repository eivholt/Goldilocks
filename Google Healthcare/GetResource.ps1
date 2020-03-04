$cred = gcloud auth print-access-token
$headers = @{ Authorization = "Bearer $cred" }
$PROJECT_ID = "iothealthcare-269209"
$LOCATION = "europe-west4"
$DATASET_ID = "IoTHealthcareDataset"
$FHIR_STORE_ID = "LocationsDataStore"
$RESOURCE_ID = "38d1495c-1e36-4789-91fc-bd09e7f42b3c"

Invoke-RestMethod `
  -Method Get `
  -Headers $headers `
  -Uri "https://healthcare.googleapis.com/v1beta1/projects/$PROJECT_ID/locations/$LOCATION/datasets/$DATASET_ID/fhirStores/$FHIR_STORE_ID/fhir/Location/$RESOURCE_ID" | ConvertTo-Json