<?xml version="1.0" encoding="UTF-8"?>
<!-- This file is part of dvisvgm -->
<!-- Copyright (C) 2015-2020 Martin Gieseking <martin.gieseking@uos.de> -->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
	<xsl:output doctype-public="-//OASIS//DTD DocBook XML V4.5//EN" doctype-system="http://www.oasis-open.org/docbook/xml/4.5/docbookx.dtd"/>

	<!-- replace default document title -->
	<xsl:template match="refentrytitle[. != ../refmiscinfo[@class='manual']]">
		<xsl:copy>
			<xsl:value-of select="../refmiscinfo[@class='manual']"/>
		</xsl:copy>
	</xsl:template>

	<!-- add static title to the synopsis section to prevent creating a dynamic one -->
	<xsl:template match="refsynopsisdiv[not(title)]">
		<xsl:copy>
			<xsl:copy-of select="@*"/>
			<title>Synopsis</title>
			<xsl:apply-templates/>
		</xsl:copy>
	</xsl:template>

	<!-- copy everything else unchanged -->
	<xsl:template match="@*|node()">
		<xsl:copy>
			<xsl:apply-templates select="@*|node()"/>
		</xsl:copy>
	</xsl:template>
</xsl:stylesheet>
