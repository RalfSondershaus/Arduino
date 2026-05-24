"""Convert a KiCad 9 BOM CSV export to the JLCPCB BOM upload format.

Workflow:
  1. In KiCad 9 PCB Editor open Fabrication Output > BOM.
  2. Export as CSV (default KiCad format).
  3. Run this script to produce a CSV that can be uploaded directly
     to JLCPCB's SMT Assembly order page.
"""
import argparse
import csv
import re
import sys


def extract_footprint(kicad_footprint):
    """Extract pure package designator (e.g. '0805') from a KiCad footprint string.

    KiCad footprints use the format 'Library:FootprintName', e.g.:
      'Resistor_SMD:R_0805_2012Metric' -> '0805'
      'Package_TO_SOT_SMD:SOT-23'      -> 'SOT-23'
    Returns the original value when no recognizable code is found.
    """
    name = kicad_footprint.split(":")[-1] if ":" in kicad_footprint else kicad_footprint

    # Match EIA 4-digit size codes (0201, 0402, 0603, 0805, 1206, ...)
    match = re.search(r'(?:^|_)(\d{4})(?:_|$)', name)
    if match:
        return match.group(1)

    # Match named package codes: SOT-23, TO-92, DIP-8, SOIC-16, etc.
    match = re.search(r'\b((?:SOT|TO|DIP|SOP|QFP|TQFP|SOIC|MSOP|SSOP)-\w+)\b', name, re.IGNORECASE)
    if match:
        return match.group(1).upper()

    return kicad_footprint


def transform_bom(input_file, output_file):
    with open(input_file, newline='', encoding="utf-8") as f:
        reader = csv.DictReader(f)
        rows = list(reader)

    jlc_rows = []

    for row in rows:
        # Skip DNP / excluded parts
        if row.get("DNP") or row.get("Exclude from BOM") or row.get("Exclude from Board"):
            continue

        refs = [r.strip() for r in row["Reference"].split(",") if r.strip()]
        quantity = len(refs)
        kicad_footprint = row.get("Footprint", "")

        jlc_rows.append({
            "Designator": " ".join(refs),
            "Quantity": quantity,
            "MPN": row.get("LCSC", ""),
            "Comment": row.get("Value", ""),
            "Footprint": extract_footprint(kicad_footprint),
            "Footprint_KiCad": kicad_footprint,
        })

    # Write JLCPCB BOM
    with open(output_file, "w", newline='', encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=["Designator", "Quantity", "MPN", "Comment", "Footprint", "Footprint_KiCad"])
        writer.writeheader()
        writer.writerows(jlc_rows)

    print(f"JLCPCB BOM written to {output_file}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description=(
            "Convert a KiCad 9 BOM CSV export to the JLCPCB BOM upload format. "
            "Export the BOM from KiCad (Fabrication Output > BOM) as CSV, then "
            "run this script to produce a file ready for JLCPCB SMT Assembly upload."
        ),
        usage="%(prog)s [-h] -o OUTPUT input"
    )
    parser.add_argument("input", help="Input KiCad BOM CSV file")
    parser.add_argument("-o", dest="output", required=True, metavar="OUTPUT",
                        help="Output JLCPCB BOM CSV file")

    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(1)

    args = parser.parse_args()
    transform_bom(args.input, args.output)
