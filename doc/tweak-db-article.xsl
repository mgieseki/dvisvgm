<?xml version="1.0"?>
<!-- This file is part of dvisvgm -->
<!-- Copyright (C) 2015-2017 Martin Gieseking <martin.gieseking@uos.de> -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output doctype-public="-//OASIS//DTD DocBook XML V4.5//EN" doctype-system="http://www.oasis-open.org/docbook/xml/4.5/docbookx.dtd"/>

	<!-- add subtitle -->
	<xsl:template match="articleinfo">
		<xsl:copy>
			<title>dvisvgm {VERSION} Manual</title>
			<subtitle>A fast DVI to SVG converter</subtitle>
			<xsl:copy-of select="*[not(self::title)]"/>
		</xsl:copy>
	</xsl:template>

	<!-- drop redundant section 'Name' -->
	<xsl:template match="section[@id='_name']"/>

	<!-- copy everything else unchanged -->
	<xsl:template match="@*|node()">
		<xsl:copy>
			<xsl:apply-templates select="@*|node()"/>
		</xsl:copy>
	</xsl:template>
</xsl:stylesheet>
