# One-time Google Sheets setup

The generator is configured to refresh this spreadsheet:

`https://docs.google.com/spreadsheets/d/1hdZdmDTvVAbuoer8FHvYYZAVdjp9NGGppLklhBIi62w/edit`

## 1. Create a Google service account

1. Open <https://console.cloud.google.com/> and create or select a project.
2. Open **APIs & Services > Library**.
3. Enable **Google Sheets API** and **Google Drive API**.
4. Open **IAM & Admin > Service Accounts**.
5. Create a service account. It does not need a project role.
6. Open the service account, then **Keys > Add key > Create new key > JSON**.

## 2. Install the key safely

Rename the downloaded JSON file to:

`google-sheets-credentials.json`

Place it beside `generate_balance_workbook.py`. This filename is ignored by
Git. Never commit or share the key.

Open the JSON and copy its `client_email`. Share the target Google Sheet with
that email address as an **Editor**.

You may keep the key elsewhere instead by setting:

```bash
export SECOND_NATURE_GOOGLE_CREDENTIALS="$HOME/private/google-sheets-credentials.json"
```

## 3. Install Python dependencies

```bash
python -m pip install openpyxl gspread google-auth
```

## 4. Generate and upload

From the pokeemerald-expansion repository:

```bash
python tools/balance_analysis/generate_balance_workbook.py
```

The local `.xlsx` remains as a backup. The same native Google spreadsheet is
then refreshed in place. Generated tabs are deliberately rebuilt on each run;
unrelated tabs are left alone.

