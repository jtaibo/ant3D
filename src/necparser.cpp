#include "necparser.h"

#include <nec2pp/libnecpp.h>

#include <fstream>

class Card {
public:
	Card(const std::string line) : _line(line)
	{}

	inline std::string getCardID()
	{
		return _line.substr(0, 2);
	}

	inline std::string getCardIFieldStr(int idx)
	{
		std::string field;
		if (idx < 1 || idx > 4) {
			std::cerr << "WARNING! Requested invalid int field (" << idx << ")" << std::endl;
			return "";
		}
		try {
			if (idx == 1) {
				// First integer field is 3 bytes long
				return _line.substr(2, 3);
			}
			else {
				return _line.substr(5 + (idx - 2) * 5, 5);
			}
		}
		catch (...) {
			return "";
		}
	}

	inline int getCardIField(int idx)
	{
		std::string val_str = getCardIFieldStr(idx);
		if (val_str.empty()) {
			return 0;
		}
		else
			return atoi(val_str.c_str());
	}

	/**
	 *	WARNING: Float field 1 starts in column 11 (as in cards with at most two I fields) 
	 *	NEC documentation call F1 to the field starting at 21 in cards with 3 or 4 I fields
	 */
	inline std::string getCardFFieldStr(int idx)
	{
		std::string field;
		if (idx < 1 || idx > 7) {
			std::cerr << "WARNING! Requested invalid float field (" << idx << ")" << std::endl;
			return "";
		}
		try {
			return _line.substr(10 + (idx - 1) * 10, 10);
		}
		catch (...) {
			return "";
		}
	}

	/**
	 *	WARNING: Float field 1 starts in column 11 (as in cards with at most two I fields)
	 *	NEC documentation call F1 to the field starting at 21 in cards with 3 or 4 I fields
	 */
	inline float getCardFField(int idx)
	{
		std::string val_str = getCardFFieldStr(idx);
		if (val_str.empty()) {
			return 0.;
		}
		else
			return (float)atof(val_str.c_str());
	}

	inline std::string getCardComment()
	{
		return _line.substr(2, _line.size()-2);
	}

private:
	std::string _line;
};

typedef enum {
	NPS_COMMENT,	///< Initial comments section
	NPS_GEOMETRY,	///< Geometry cards
	NPS_CONTROL,	///< Program control cards
	NPS_END
} NECParserState;

#define NOT_IMPLEMENTED_YET(str) \
	std::cerr << "WARNING: This feature not implemented yet! (" << str << ")" << std::endl; \
	exit(-1);

nec_context *create_nec_context_from_file(const std::string &filename)
{
	// Create (and initialize) NEC context
	nec_context *nec = nec_create();

	std::cout << "Parsing file " << filename << std::endl;
	NECParserState parser_state = NPS_COMMENT;
	std::ifstream input(filename);
	std::string line;
	while (std::getline(input, line)) {
//		std::cout << "Line: " << line << std::endl;
		Card card(line);
		std::string card_type = card.getCardID();
//		std::cout << "Card type: " << card_type << std::endl;

		switch (parser_state) {
		case NPS_COMMENT:
			if (card_type == "CM") {
				// Comment
				std::cout << card.getCardComment() << std::endl;
			}
			else if (card_type == "CE") {
				// Comments end
				std::cout << card.getCardComment() << std::endl;
				parser_state = NPS_GEOMETRY;
			}
			else {
				std::cerr << "ERROR. Unexpected card at the initial comments section: " << card_type << std::endl;
				nec_delete(nec);
				return NULL;
			}
			break;
		case NPS_GEOMETRY:
			if (card_type == "GA") {
				// Wire Arc Specification
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "GF") {
				// Read Numerical Green’s Function File
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "GH") {
				// Helix/Spiral Specification
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "GM") {
				// Coordinate Transformation
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "GR") {
				// Generate Cylindrical Structure
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "GS") {
				// Scale Structure Dimensions
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "GW") {
				// Wire Specification
				int tag_id = card.getCardIField(1);
				int segment_count = card.getCardIField(2);
				float x1 = card.getCardFField(1);
				float y1 = card.getCardFField(2);
				float z1 = card.getCardFField(3);
				float x2 = card.getCardFField(4);
				float y2 = card.getCardFField(5);
				float z2 = card.getCardFField(6);
				float radius = card.getCardFField(7);
				float rdel = 1.;
				float rrad = 1.;
				if (radius == 0.0f) {
					if (std::getline(input, line)) {
						Card x_card(line);
						std::string x_card_type = x_card.getCardID();
						if (x_card_type == "GC") {
							rdel = x_card.getCardFField(1);
							float rad1 = x_card.getCardFField(2);
							float rad2 = x_card.getCardFField(3);
							rrad = (float)pow(rad2 / rad1, 1./(segment_count-1));
							radius = rad1;
						}
						else {
							std::cerr << "ERROR. Unexpected card (" << x_card_type << ") awaiting GC" << std::endl;
							nec_delete(nec);
							return NULL;
						}
					}
					else {
						std::cerr << "ERROR. Unexpected end of file!" << std::endl;
						nec_delete(nec);
						return NULL;
					}
				}
				nec->wire(tag_id, segment_count, x1, y1, z1, x2, y2, z2, radius, rdel, rrad);
			}
			else if (card_type == "GX") {
				// Reflection in Coordinate Planes
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "SP") {
				// Surface Patch
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "SM") {
				// Multiple Patch Surface
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "GE") {
				// End of geometry definition
				int gp_flag = card.getCardIField(1);
				nec->geometry_complete( gp_flag );
				parser_state = NPS_CONTROL;
			}
			else {
				std::cerr << "ERROR. Unexpected card at the geometry section: " << card_type << std::endl;
				nec_delete(nec);
				return NULL;
			}
			break;
		case NPS_CONTROL:
			if (card_type == "CP") {
				// Maximum Coupling Calculation
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "EK") {
				// Extended Thin-Wire Kernel
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "EX") {
				// Excitation
				excitation_type itmp1 = (excitation_type)card.getCardIField(1);
				int itmp2 = card.getCardIField(2);
				int itmp3 = card.getCardIField(3);
				int itmp4 = card.getCardIField(4);
				float tmp1 = card.getCardFField(2);
				float tmp2 = card.getCardFField(3);
				float tmp3 = card.getCardFField(4);
				float tmp4 = card.getCardFField(5);
				float tmp5 = card.getCardFField(6);
				float tmp6 = card.getCardFField(7);
				nec->ex_card(itmp1, itmp2, itmp3, itmp4, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6);
			}
			else if (card_type == "FR") {
				// Frequency
				int ifrq = card.getCardIField(1);	
				int nfrq = card.getCardIField(2);
				float freq_mhz = card.getCardFField(2);
				float del_freq = card.getCardFField(3);
				nec->fr_card(ifrq, nfrq, freq_mhz, del_freq);
			}
			else if (card_type == "GD") {
				// Additional Ground Parameters
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "GN") {
				// Ground Parameters
				int ground_type = card.getCardIField(1);	// IPERF
				int rad_wire_count = card.getCardIField(2);	// NRADL
				float tmp1 = card.getCardFField(2);		// EPSE
				float tmp2 = card.getCardFField(3);		// SIG
				float tmp3 = card.getCardFField(4);
				float tmp4 = card.getCardFField(5);
				float tmp5 = card.getCardFField(6);
				float tmp6 = card.getCardFField(7);
				nec->gn_card(ground_type, rad_wire_count, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6);
			}
			else if (card_type == "KH") {
				// Interaction Approximation Range
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "LD") {
				// Loading
				int itmp1 = card.getCardIField(1);	// LDTYP
				int itmp2 = card.getCardIField(2);	// LDTAG
				int itmp3 = card.getCardIField(3);	// LDTAGF
				int itmp4 = card.getCardIField(4);	// LDTAGT
				float tmp1 = card.getCardFField(2);	// ZLR
				float tmp2 = card.getCardFField(3);	// ZLI
				float tmp3 = card.getCardFField(4);	// ZLC
				nec->ld_card(itmp1, itmp2, itmp3, itmp4, tmp1, tmp2, tmp3);
			}
			else if (card_type == "NE") {
				// Near Fields
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "NH") {
				// Near Fields
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "NT") {
				// Networks
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "NX") {
				// Next Structure
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "PQ") {
				// Print Control for Charge on Wires
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "PT") {
				// Page Title / Print Control for Current on Wires
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "RP") {
				// Radiation Pattern
				int calc_mode = card.getCardIField(1);
				int n_theta = card.getCardIField(2);
				int n_phi = card.getCardIField(3);
				std::string xnda = card.getCardIFieldStr(4);
				int output_format = xnda[1] - '0';	// X
				int normalization = xnda[2] - '0';	// N
				int D = xnda[3] - '0';				// D
				int A = xnda[4] - '0';				// A
				float theta0 = card.getCardFField(2);
				float phi0 = card.getCardFField(3);
				float delta_theta = card.getCardFField(4);
				float delta_phi = card.getCardFField(5);
				float radial_distance = card.getCardFField(6);
				float gain_norm = card.getCardFField(7);
				nec->rp_card(calc_mode, n_theta, n_phi, output_format, normalization, D, A, theta0, phi0, delta_theta, delta_phi, radial_distance, gain_norm);
			}
			else if (card_type == "TL") {
				// Transmission Line
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "WG") {
				// Write NGF File
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "XQ") {
				// Execute
				NOT_IMPLEMENTED_YET(card_type)
			}
			else if (card_type == "EN") {
				// End of Run
				parser_state = NPS_END;
			}
			else {
				std::cerr << "ERROR. Unexpected card at the program control section: " << card_type << std::endl;
				nec_delete(nec);
				return NULL;
			}
			break;
		case NPS_END:
			std::cerr << "ERROR. No cards allowed after End of Run (EN) card. Found " << card_type << std::endl;
			nec_delete(nec);
			return NULL;
			break;
		}
	}

	if (parser_state != NPS_END) {
		std::cerr << "ERROR. Input file parsing incomplete" << std::endl;
		nec_delete(nec);
		return NULL;
	}

	input.close();

	return nec;
}
