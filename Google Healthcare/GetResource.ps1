﻿$cred = gcloud auth print-access-token
$headers = @{ Authorization = "Bearer $cred" }
$PROJECT_ID = "iothealthcare-269209"
$LOCATION = "europe-west4"
$DATASET_ID = "IoTHealthcareDataset"
$FHIR_STORE_ID = "ResourcesDataStore"
$RESOURCE_ID = "e3b95e17-3195-4a19-87eb-409cdeb90301"

Invoke-RestMethod `
  -Method Get `
  -Headers $headers `
  -Uri "https://healthcare.googleapis.com/v1beta1/projects/$PROJECT_ID/locations/$LOCATION/datasets/$DATASET_ID/fhirStores/$FHIR_STORE_ID/fhir/Location/$RESOURCE_ID" | ConvertTo-Json